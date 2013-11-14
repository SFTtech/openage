#include "engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <FTGL/ftgl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "console.h"
#include "font.h"
#include "texture.h"
#include "../log/log.h"
#include "../util/error.h"
#include "../util/fps.h"
#include "../util/strings.h"

namespace openage {
namespace engine {

//global engine variables; partially accesible via engine.h
SDL_GLContext glcontext;
SDL_Window *window;

FTGLTextureFont *t_font;

Console *console;

noparam_method_ptr on_engine_tick;
noparam_method_ptr draw_method;
input_handler_ptr input_handler;


bool running;

coord::sdl window_size = {800, 600};
coord::phys camera_pos_phys = {0, 0, 0};
coord::sdl camera_pos_sdl = {400, 300};

util::FrameCounter *fpscounter;
bool console_activated = false;

/**
initialize the openage game engine.

this creates the SDL window, the opengl context, reads shaders, the fps counter, ...
*/
void init(noparam_method_ptr on_engine_tick, noparam_method_ptr draw_method, input_handler_ptr input_handler) {

	//set global random seed
	srand(time(NULL));

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		throw util::Error("SDL initialization: %s", SDL_GetError());
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window = SDL_CreateWindow("openage", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_size.x, window_size.y, SDL_WINDOW_OPENGL);

	if (window == nullptr) {
		throw util::Error("SDL window creation: %s", SDL_GetError());
	}


	// load support for the PNG image formats, jpg: IMG_INIT_JPG | IMG_INIT_PNG
	int wanted_image_formats = IMG_INIT_PNG;
	int sdlimg_inited = IMG_Init(wanted_image_formats);
	if ((sdlimg_inited & wanted_image_formats) != wanted_image_formats) {
		throw util::Error("Failed to init PNG support: %s", IMG_GetError());
	}

	glcontext = SDL_GL_CreateContext(window);

	//initialize glew, for shaders n stuff
	GLenum glew_state = glewInit();
	if (glew_state != GLEW_OK) {
		throw util::Error("GLEW initialization failed");
	}
	if (!GLEW_VERSION_2_1) {
		throw util::Error("OpenGL 2.1 not available");
	}

	//vsync on
	SDL_GL_SetSwapInterval(1);

	//enable alpha blending
	glEnable(GL_BLEND);

	//order of drawing relevant for depth
	//what gets drawn last is displayed on top.
	glDisable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	engine::on_engine_tick = on_engine_tick;
	engine::draw_method = draw_method;
	engine::input_handler = input_handler;


	const char *font_filename = get_font_filename("DejaVu Serif", "Book");
	t_font = new FTGLTextureFont(font_filename);
	delete[] font_filename;

	if(t_font->Error())
		throw util::Error("failed creating the dejavu font with ftgl");
	t_font->FaceSize(20);

	//initialize the visual debug console
	console = new Console(util::Color(255, 255, 255, 180), util::Color(0, 0, 0, 255));

	//initialize the fps counter
	fpscounter = new util::FrameCounter();
}

/**
destroys everything created upon creation of the engine.

deletes opengl context, the SDL window, and engine variables.
*/
void destroy() {
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	delete fpscounter;
	delete console;
	delete t_font;
	IMG_Quit();
	SDL_Quit();
}

/**
method that's called when the SDL window is resized.

this adjusts the opengl viewport and calls for visible area recalculation.
*/
void engine_window_resized(unsigned w, unsigned h) {
	//update window size
	window_size.x = w;
	window_size.y = h;
	//update camera SDL coordinates
	camera_pos_sdl.x = w / 2;
	camera_pos_sdl.y = h / 2;
	//update console window size
	console->set_winsize(w, h);
	//update OpenGL viewport and projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);

	// set orthographic projection: left, right, bottom, top, nearVal, farVal
	glOrtho(0, w, h, 0, 9001, -1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	log::dbg("engine window has been resized to %ux%u\n", w, h);
}

/**
handles some keys directly in preference over the game input handler.

this is mainly for high-priority events.
at the moment, it handles window resizing events,
and opening/closing the in-game terminal.
*/
void engine_input_handler(SDL_Event *e) {
	switch(e->type) {
	case SDL_WINDOWEVENT:
		switch(e->window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			engine_window_resized(e->window.data1, e->window.data2);
			break;
		}
		break;

	case SDL_KEYUP:
		break;

	case SDL_KEYDOWN:
		switch (((SDL_KeyboardEvent *) e)->keysym.sym) {
		case SDLK_BACKQUOTE:
			console_activated = !console_activated;
			break;
		}
		break;
	}
}

/**
the main engine loop.

the loop invokes fps counting, SDL event handling,
view translation, and calling the main draw_method.
*/
void loop() {
	int glerrorstate = 0;
	running = true;
	SDL_Event e;

	while (running) {

		fpscounter->frame();


		//clear the framebuffer to black
		//in the future, we might disable it for lazy drawing
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		while (SDL_PollEvent(&e)) {
			//first, handle the event ourselves (e.g.: detect window resize)
			engine_input_handler(&e);

			if (!console_activated) {
				//now, give the event to the user of the engine
				input_handler(&e);
			}
			else {
				//or, give it to the console, if that is opened
				console->input_handler(&e);
			}
		}

		//tick the gamelogic here
		//tick(magic);
		//call the registered callback method
		on_engine_tick();

		glPushMatrix();
		{
			//after this transformation, it is possible to directly
			//draw in the camera coordinate system.
			glTranslatef(camera_pos_sdl.x, camera_pos_sdl.y, 0);
			draw_method();
		}
		glPopMatrix();

		if (console_activated) {
			console->draw();
		}

		glPushMatrix();
		{
			//here, it is possible to directly draw in the HUD coordinate
			//system

			//draw the FPS counter
			glPushMatrix();
			{
				//top left corner
				glTranslatef(15, 30, 0);
				glScalef(1,-1,1);
				//white
				glColor4f(1.0, 1.0, 1.0, 1.0);
				char *fpstext = util::format("%.1f fps", fpscounter->fps);
				t_font->Render(fpstext);
				delete[] fpstext;
			}
			glPopMatrix();
		}
		glPopMatrix();


		glerrorstate = glGetError();
		if (glerrorstate != GL_NO_ERROR) {

			const char *errormsg;

			//generate error message
			switch (glerrorstate) {
			case GL_INVALID_ENUM:
				//An unacceptable value is specified for an enumerated argument.
				//The offending command is ignored
				//and has no other side effect than to set the error flag.
				errormsg = "invalid enum passed to opengl call";
				break;
			case GL_INVALID_VALUE:
				//A numeric argument is out of range.
				//The offending command is ignored
				//and has no other side effect than to set the error flag.
				errormsg = "invalid value passed to opengl call";
				break;
			case GL_INVALID_OPERATION:
				//The specified operation is not allowed in the current state.
				//The offending command is ignored
				//and has no other side effect than to set the error flag.
				errormsg = "invalid operation performed during some state";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				//The framebuffer object is not complete. The offending command
				//is ignored and has no other side effect than to set the error flag.
				errormsg = "invalid framebuffer operation";
				break;
			case GL_OUT_OF_MEMORY:
				//There is not enough memory left to execute the command.
				//The state of the GL is undefined,
				//except for the state of the error flags,
				//after this error is recorded.
				errormsg = "out of memory, wtf?";
				break;
			case GL_STACK_UNDERFLOW:
				//An attempt has been made to perform an operation that would
				//cause an internal stack to underflow.
				errormsg = "stack underflow";
				break;
			case GL_STACK_OVERFLOW:
				//An attempt has been made to perform an operation that would
				//cause an internal stack to overflow.
				errormsg = "stack overflow";
				break;
			}
			throw util::Error("OpenGL error state after running draw method: %d\n\t%s", glerrorstate, errormsg);
		}

		SDL_GL_SwapWindow(window);
	}
}

} //namespace engine
} //namespace openage
