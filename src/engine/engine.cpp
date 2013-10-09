#include "engine.h"

#include <stdio.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <FTGL/ftgl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "texture.h"
#include "console.h"
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

draw_method_ptr draw_method;
input_handler_ptr input_handler;

bool running;
unsigned window_x = 800;
unsigned window_y = 600;

util::FrameCounter fpscounter;
bool console_activated = false;



void init(draw_method_ptr draw_method, input_handler_ptr input_handler) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		throw util::Error("SDL initialization: %s", SDL_GetError());
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window = SDL_CreateWindow("openage", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_x, window_y, SDL_WINDOW_OPENGL);

	if (window == nullptr) {
		throw util::Error("SDL window creation: %s", SDL_GetError());
	}


	// load support for the JPG and PNG image formats
	int wanted_image_formats = IMG_INIT_JPG | IMG_INIT_PNG;
	int sdlimg_inited = IMG_Init(wanted_image_formats);
	if ((sdlimg_inited & wanted_image_formats) != wanted_image_formats) {
		throw util::Error("Failed to init JPG/PNG support: %s", IMG_GetError());
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

	engine::draw_method = draw_method;
	engine::input_handler = input_handler;

	//load font for drawing texts
	// TODO: don't hardcode font path, make it dynamic (sounds like a job for cmake)..
	t_font = new FTGLTextureFont("/usr/share/fonts/dejavu/DejaVuSerif.ttf");
	if(t_font->Error())
		throw util::Error("failed creating the dejavu font with ftgl");
	t_font->FaceSize(20);

	//initialize the visual debug console
	console = new Console(util::Color(255, 255, 255, 180), util::Color(0, 0, 0, 255));
}

void destroy() {
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	delete console;
	delete t_font;
	IMG_Quit();
	SDL_Quit();
}

void engine_window_resized(unsigned w, unsigned h) {

	//store the current window size
	window_x = w;
	window_y = h;

	console->set_winsize(w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);

	// set orthographic projection: left, right, bottom, top, nearVal, farVal
	glOrtho(0, w, 0, h, 9001, -1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	log::dbg("engine window has been resized to %ux%u\n", w, h);
}

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

void loop() {
	int glerrorstate = 0;
	running = true;
	SDL_Event e;

	while (running) {

		fpscounter.frame();


		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		//tick the gamelogic here
		//tick(magic);

		while (SDL_PollEvent(&e)) {
			//first, handle the event ourselves (e.g.: detect window resize)
			engine_input_handler(&e);

			if (!console_activated) {
				//now, give the event to the user of the engine
				input_handler(&e);
			}
			else {
				console->input_handler(&e);
			}
		}

		draw_method();

		if (console_activated) {
			console->draw();
		}

		glPushMatrix();
		{
			glTranslatef(engine::window_x - 100, 15, 0);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			char *fpstext = util::format("%.1f fps", fpscounter.fps);
			t_font->Render(fpstext);
			delete[] fpstext;
		}
		glPopMatrix();


		glerrorstate = glGetError();
		if (glerrorstate != 0) {
			throw util::Error("OpenGL error state after running draw method: %d", glerrorstate);
		}

		SDL_GL_SwapWindow(window);
	}
}

} //namespace engine
} //namespace openage
