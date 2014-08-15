#include "engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <FTGL/ftgl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "callbacks.h"
#include "texture.h"
#include "log.h"
#include "util/color.h"
#include "util/error.h"
#include "util/fps.h"
#include "util/opengl.h"
#include "util/strings.h"


/**
 * stores all things that have to do with the game.
 *
 * this includes textures, objects, terrain, etc.
 * movement and transformation stuff, and actually everything that
 * makes the game work lies in here...
 */
namespace openage {

// engine singleton instance allocation
Engine *Engine::instance = nullptr;

void Engine::create(util::Dir *data_dir, const char *windowtitle) {
	// only create the singleton instance if it was not created before..
	if (Engine::instance == nullptr) {
		// reset the pointer to the new engine
		Engine::instance = new Engine(data_dir, windowtitle);
	}
	else {
		throw util::Error{"you tried to create another singleton instance!!111"};
	}
}

void Engine::destroy() {
	if (Engine::instance == nullptr) {
		throw util::Error{"you tried to destroy a nonexistant engine."};
	}
	else {
		delete Engine::instance;
	}
}

Engine &Engine::get() {
	return *Engine::instance;
}


Engine::Engine(util::Dir *data_dir, const char *windowtitle)
	:
	running(false),
	window_size{800, 600},
	camgame_phys{10 * coord::settings::phys_per_tile, 10 * coord::settings::phys_per_tile, 0},
	camgame_window{400, 300},
	camhud_window{0, 600},
	tile_halfsize{48, 24},  // TODO: get from convert script
	data_dir(data_dir),
	audio_manager{48000, AUDIO_S16LSB, 2, 4096}
{

	// enqueue the engine's own input handler to the
	// execution list.
	this->register_input_action(&this->input_handler);
	this->input_handler.register_resize_action(this);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		throw util::Error("SDL video initialization: %s", SDL_GetError());
	} else {
		log::msg("initialized SDL video subsystems.");
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	int32_t window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;
	this->window = SDL_CreateWindow(
		windowtitle,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		this->window_size.x,
		this->window_size.y,
		window_flags
	);

	if (this->window == nullptr) {
		throw util::Error("Failed creating SDL window: %s", SDL_GetError());
	}

	// load support for the PNG image formats, jpg bit: IMG_INIT_JPG
	int wanted_image_formats = IMG_INIT_PNG;
	int sdlimg_inited = IMG_Init(wanted_image_formats);
	if ((sdlimg_inited & wanted_image_formats) != wanted_image_formats) {
		throw util::Error("Failed to init PNG support: %s", IMG_GetError());
	}

	this->glcontext = SDL_GL_CreateContext(this->window);

	if (this->glcontext == nullptr) {
		throw util::Error("Failed creating OpenGL context: %s", SDL_GetError());
	}

	// initialize glew, for shaders n stuff
	GLenum glew_state = glewInit();
	if (glew_state != GLEW_OK) {
		throw util::Error("GLEW initialization failed");
	}
	if (!GLEW_VERSION_2_1) {
		throw util::Error("OpenGL 2.1 not available");
	}

	// to quote the standard doc:
	// 'The value gives a rough estimate
	// of the largest texture that the GL can handle'
	// -> wat?
	// anyways, we need at least 1024x1024.
	int max_texture_size;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
	log::dbg("Maximum supported texture size: %d", max_texture_size);
	if (max_texture_size < 1024) {
		throw util::Error("Maximum supported texture size too small: %d", max_texture_size);
	}

	// vsync on
	SDL_GL_SetSwapInterval(1);

	// enable alpha blending
	glEnable(GL_BLEND);

	// order of drawing relevant for depth
	// what gets drawn last is displayed on top.
	glDisable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// initialize debug text font
	this->dejavuserif20 = new Font{"DejaVu Serif", "Book", 20};

	// initialize job manager with cpucount-2 worker threads
	int number_of_worker_threads = SDL_GetCPUCount() - 2;
	if (number_of_worker_threads <= 0) {
		number_of_worker_threads = 1;
	}
	this->job_manager = new job::JobManager{number_of_worker_threads};

	// initialize audio
	auto devices = audio::AudioManager::get_devices();
	if (devices.empty()) {
		throw util::Error{"No audio devices found"};
	}
}

Engine::~Engine() {
	delete this->dejavuserif20;
	delete this->job_manager;
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
}

bool Engine::on_resize(coord::window new_size) {
	log::dbg("engine window resize to: %hdx%hd\n", new_size.x, new_size.y);

	// update engine window size
	this->window_size = new_size;

	// update camgame window position, set it to center.
	this->camgame_window = this->window_size / 2;

	// update camhud window position
	this->camhud_window = {0, (coord::pixel_t) this->window_size.y};

	// reset previous projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// update OpenGL viewport: the renderin area
	glViewport(0, 0, this->window_size.x, this->window_size.y);

	// set orthographic projection: left, right, bottom, top, near_val, far_val
	glOrtho(0, this->window_size.x, 0, this->window_size.y, 9001, -1);

	// reset the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return true;
}

bool Engine::draw_fps() {
	//draw FPS counter
	util::col {255, 255, 255, 255}.use();

	this->dejavuserif20->render(
		this->window_size.x - 100, 15,
		"%.1f fps", this->fpscounter.fps
	);

	return true;
}

void Engine::save_screenshot(const char* filename) {
	log::msg("saving screenshot to %s...", filename);

	int32_t rmask, gmask, bmask, amask;
	rmask = 0x000000FF;
	gmask = 0x0000FF00;
	bmask = 0x00FF0000;
	amask = 0xFF000000;

	SDL_Surface *screen = SDL_CreateRGBSurface(
		SDL_SWSURFACE,
		this->window_size.x,
		this->window_size.y,
		32,
		rmask, gmask, bmask, amask);

	size_t pxcount = screen->w * screen->h;
	uint32_t *pxdata = new uint32_t[pxcount];

	glReadPixels(
		0, 0,
		this->window_size.x, this->window_size.y,
		GL_RGBA, GL_UNSIGNED_BYTE, pxdata
	);

	uint32_t *surface_pxls = (uint32_t *)screen->pixels;

	// we need to invert all pixel rows, but leave column order the same.
	for (ssize_t row = 0; row < screen->h; row++) {
		ssize_t irow = screen->h - 1 - row;
		for (ssize_t col = 0; col < screen->w; col++) {
			uint32_t pxl = pxdata[irow * screen->w + col];

			// TODO: store the alpha channels in the screenshot, is buggy at the moment..
			surface_pxls[row * screen->w + col] = pxl | 0xFF000000;
		}
	}

	delete[] pxdata;

	// call sdl_image for saving the screenshot to png
	IMG_SavePNG(screen, filename);
	SDL_FreeSurface(screen);
}

void Engine::run() {
	this->job_manager->start();
	this->running = true;
	this->loop();
	this->running = false;
}

void Engine::stop() {
	this->job_manager->stop();
	this->running = false;
}

void Engine::loop() {
	SDL_Event event;

	while (this->running) {
		this->fpscounter.frame();

		while (SDL_PollEvent(&event)) {
			for (auto &action : this->on_input_event) {
				if (false == action->on_input(&event)) {
					break;
				}
			}
		}

		// call engine tick callback methods
		for (auto &action : this->on_engine_tick) {
			if (false == action->on_tick()) {
				break;
			}
		}

		// clear the framebuffer to black
		// in the future, we might disable it for lazy drawing
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glPushMatrix(); {
			// set the framebuffer up for camgame rendering
			glTranslatef(camgame_window.x, camgame_window.y, 0);

			// invoke all game drawing handlers
			for (auto &action : this->on_drawgame) {
				if (false == action->on_draw()) {
					break;
				}
			}
		}
		glPopMatrix();

		util::gl_check_error();

		glPushMatrix(); {
			// the hud coordinate system is automatically established

			// draw the fps overlay
			this->draw_fps();

			// invoke all hud drawing callback methods
			for (auto &action : this->on_drawhud) {
				if (false == action->on_drawhud()) {
					break;
				}
			}
		}
		glPopMatrix();

		util::gl_check_error();

		// the rendering is done
		// swap the drawing buffers to actually show the frame
		SDL_GL_SwapWindow(window);
	}
}

void Engine::register_input_action(InputHandler *handler) {
	this->on_input_event.push_back(handler);
}

void Engine::register_tick_action(TickHandler *handler) {
	this->on_engine_tick.push_back(handler);
}

void Engine::register_drawhud_action(HudHandler *handler) {
	this->on_drawhud.push_back(handler);
}

void Engine::register_draw_action(DrawHandler *handler) {
	this->on_drawgame.push_back(handler);
}

void Engine::register_resize_action(ResizeHandler *handler) {
	this->input_handler.register_resize_action(handler);
}

util::Dir *Engine::get_data_dir() {
	return this->data_dir;
}

job::JobManager *Engine::get_job_manager() {
	return this->job_manager;
}

audio::AudioManager &Engine::get_audio_manager() {
	return this->audio_manager;
}

CoreInputHandler &Engine::get_input_handler() {
	return this->input_handler;
}

unsigned int Engine::lastframe_msec() {
	return this->fpscounter.msec_lastframe;
}



} //namespace openage
