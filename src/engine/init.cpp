#include "init.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "engine.h"
#include "callbacks.h"
#include "font.h"
#include "texture.h"
#include "util/error.h"
#include "input.h"


namespace engine {

void init(const char *windowtitle) {
	//set global random seed
	srand(time(NULL));

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		throw Error("SDL initialization: %s", SDL_GetError());
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	int32_t window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;
	window = SDL_CreateWindow(windowtitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_size.x, window_size.y, window_flags);

	if (window == nullptr) {
		throw Error("SDL window creation: %s", SDL_GetError());
	}


	// load support for the PNG image formats, jpg: IMG_INIT_JPG | IMG_INIT_PNG
	int wanted_image_formats = IMG_INIT_PNG;
	int sdlimg_inited = IMG_Init(wanted_image_formats);
	if ((sdlimg_inited & wanted_image_formats) != wanted_image_formats) {
		throw Error("Failed to init PNG support: %s", IMG_GetError());
	}

	glcontext = SDL_GL_CreateContext(window);

	if (glcontext == nullptr) {
		throw Error("Failed to create OpenGL context!");
	}

	//initialize glew, for shaders n stuff
	GLenum glew_state = glewInit();
	if (glew_state != GLEW_OK) {
		throw Error("GLEW initialization failed");
	}
	if (!GLEW_VERSION_2_1) {
		throw Error("OpenGL 2.1 not available");
	}

	//vsync on
	SDL_GL_SetSwapInterval(1);

	//enable alpha blending
	glEnable(GL_BLEND);

	//order of drawing relevant for depth
	//what gets drawn last is displayed on top.
	glDisable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	fonts::dejavuserif20 = new Font("DejaVu Serif", "Book", 20);

	//initialize the fps counter
	fpscounter = new util::FrameCounter();

	callbacks::on_resize.push_back(engine::handle_window_resize);
	callbacks::on_input.push_back(engine::input::handler);
	callbacks::on_drawhud.push_back(engine::draw_hud);

	//initialize audio
	auto devices = audio::AudioManager::get_devices();
	if (devices.empty()) {
		throw Error{"No audio devices found"};
	}

	audio_manager = new audio::AudioManager(48000, AUDIO_S16LSB, 2, 4096);
}

/**
destroys everything created upon creation of the engine.

deletes opengl context, the SDL window, and engine variables.
*/
void destroy() {
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	delete fpscounter;
	delete fonts::dejavuserif20;
	IMG_Quit();
	SDL_Quit();
}

} //namespace engine
