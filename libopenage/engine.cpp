// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#include "engine.h"

#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <epoxy/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "config.h"
#include "error/error.h"
#include "error/gl_debug.h"
#include "gamestate/game_main.h"
#include "gamestate/generator.h"
#include "gui/gui.h"
#include "log/log.h"
#include "renderer/font/font.h"
#include "renderer/font/font_manager.h"
#include "renderer/text.h"
#include "texture.h"
#include "util/color.h"
#include "util/fps.h"
#include "util/opengl.h"
#include "util/strings.h"
#include "util/timer.h"

/**
 * Main openage namespace to store all things that make the have to do with the game.
 *
 * Game entity management, graphics drawing, gui stuff, input handling etc.
 * So basically everything that makes the game work lies in here...
 */
namespace openage {


/*
 * temporary global coord struct as used by the coord subsystem
 * TODO: remove and deglobalize!
 */
coord_data coord_global_tmp_TODO;


Engine::Engine(const util::Path &root_dir,
               int32_t fps_limit,
               bool gl_debug,
               const char *windowtitle)
	:
	OptionNode{"Engine"},
	running{false},
	drawing_debug_overlay{this, "drawing_debug_overlay", true},
	drawing_huds{this, "drawing_huds", true},
	root_dir{root_dir},
	job_manager{SDL_GetCPUCount()},
	qml_info{this, root_dir["assets"]},
	cvar_manager{root_dir["cfg"]},
	action_manager{&this->input_manager, &this->cvar_manager},
	audio_manager{&this->job_manager},
	input_manager{&this->action_manager},
	profiler{this},
	coord{coord_global_tmp_TODO},
	gui_link{} {


	if (fps_limit > 0) {
		this->ns_per_frame = 1e9 / fps_limit;
	} else {
		this->ns_per_frame = 0;
	}

	this->font_manager = std::make_unique<renderer::FontManager>();
	for (uint32_t size : {12, 20}) {
		fonts[size] = this->font_manager->get_font("DejaVu Serif", "Book", size);
	}

	// temporary log to the filesystem.
	// some housekeeping should be implemented as this file is only ever appended to
	this->logsink_file = std::make_unique<log::FileSink>("/tmp/openage-log", true);

	// enqueue the engine's own input handler to the
	// execution list.
	this->register_resize_action(this);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		throw Error(MSG(err) << "SDL video initialization: " << SDL_GetError());
	} else {
		log::log(MSG(info) << "Initialized SDL video subsystems.");
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	int32_t window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;
	this->window = SDL_CreateWindow(
		windowtitle,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		this->coord.window_size.x,
		this->coord.window_size.y,
		window_flags
	);

	if (this->window == nullptr) {
		throw Error(MSG(err) << "Failed to create SDL window: " << SDL_GetError());
	}

	// load support for the PNG image formats, jpg bit: IMG_INIT_JPG
	int wanted_image_formats = IMG_INIT_PNG;
	int sdlimg_inited = IMG_Init(wanted_image_formats);
	if ((sdlimg_inited & wanted_image_formats) != wanted_image_formats) {
		throw Error(MSG(err) << "Failed to init PNG support: " << IMG_GetError());
	}

	if (gl_debug)
		this->glcontext = error::create_debug_context(this->window);
	else
		this->glcontext = SDL_GL_CreateContext(this->window);

	if (this->glcontext == nullptr) {
		throw Error(MSG(err) << "Failed creating OpenGL context: " << SDL_GetError());
	}

	// check the OpenGL version, for shaders n stuff
	if (!epoxy_is_desktop_gl() || epoxy_gl_version() < 21) {
		throw Error(MSG(err) << "OpenGL 2.1 not available");
	}

	// to quote the standard doc:
	// 'The value gives a rough estimate
	// of the largest texture that the GL can handle'
	// -> wat?
	// anyways, we need at least 1024x1024.
	int max_texture_size;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
	log::log(MSG(dbg) << "Maximum supported texture size: " << max_texture_size);
	if (max_texture_size < 1024) {
		throw Error(MSG(err) << "Maximum supported texture size too small: " << max_texture_size);
	}

	int max_texture_units;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_texture_units);
	log::log(MSG(dbg) << "Maximum supported texture units: " << max_texture_units);
	if (max_texture_units < 2) {
		throw Error(MSG(err) << "Your GPU has not enough texture units: " << max_texture_units);
	}

	// vsync on
	SDL_GL_SetSwapInterval(1);

	// enable alpha blending
	glEnable(GL_BLEND);

	// order of drawing relevant for depth
	// what gets drawn last is displayed on top.
	glDisable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//// -- initialize the gui
	util::Path qml_root = this->root_dir / "assets" / "qml";
	if (not qml_root.is_dir()) {
		throw Error{ERR << "could not find qml root folder " << qml_root};
	}

	util::Path qml_root_file = qml_root / "main.qml";
	if (not qml_root_file.is_file()) {
		throw Error{ERR << "could not find main.qml file " << qml_root_file};
	}

	// TODO: in order to support qml-mods, the fslike and filelike
	//       library has to be integrated into qt. For now,
	//       figure out the absolute paths here and pass them in.

	std::string qml_root_str = qml_root.resolve_native_path();
	std::string qml_root_file_str = qml_root_file.resolve_native_path();

	this->gui = std::make_unique<gui::GUI>(
		this->window,                // sdl window for the gui
		qml_root_file_str,           // entry qml file, absolute path.
		qml_root_str,                // directory to watch for qml file changes
		&this->qml_info              // qml data: Engine *, the data directory, ...
	);
	//// -- gui initialization


	// register the engines input manager
	this->register_input_action(&this->input_manager);


	// initialize engine related global keybinds
	auto &global_input_context = this->get_input_manager().get_global_context();

	input::ActionManager &action = this->get_action_manager();
	global_input_context.bind(action.get("STOP_GAME"), [this](const input::action_arg_t &) {
		this->stop();
	});
	global_input_context.bind(action.get("TOGGLE_HUD"), [this](const input::action_arg_t &) {
		this->drawing_huds.value = !this->drawing_huds.value;
	});
	global_input_context.bind(action.get("SCREENSHOT"), [this](const input::action_arg_t &) {
		this->get_screenshot_manager().save_screenshot();
	});
	global_input_context.bind(action.get("TOGGLE_DEBUG_OVERLAY"), [this](const input::action_arg_t &) {
		this->drawing_debug_overlay.value = !this->drawing_debug_overlay.value;
	});
	global_input_context.bind(action.get("TOGGLE_PROFILER"), [this](const input::action_arg_t &) {
		if (this->external_profiler.currently_profiling) {
			this->external_profiler.stop();
			this->external_profiler.show_results();
		} else {
			this->external_profiler.start();
		}
	});
	global_input_context.bind(input::event_class::MOUSE, [this](const input::action_arg_t &arg) {
		if (arg.e.cc.has_class(input::event_class::MOUSE_MOTION) &&
			this->get_input_manager().is_down(input::event_class::MOUSE_BUTTON, 2)) {
			this->move_phys_camera(arg.motion.x, arg.motion.y);
			return true;
		}
		return false;
	});

	this->text_renderer = std::make_unique<renderer::TextRenderer>();
	this->unit_selection = std::make_unique<UnitSelection>(this);
}


Engine::~Engine() {
	this->profiler.unregister_all();

	// deallocate the gui system
	// this looses the opengl context in the qtsdl::GuiRenderer
	// deallocation (of the QtOpenGLContext).
	// so no gl* functions can be called any more.
	this->gui.reset(nullptr);

	SDL_GL_DeleteContext(this->glcontext);
	SDL_DestroyWindow(this->window);
	IMG_Quit();
	SDL_Quit();
}


bool Engine::on_resize(coord::window new_size) {
	log::log(MSG(dbg) << "engine window resize to "
	         << new_size.x << "x" << new_size.y);

	// update engine window size
	this->coord.window_size = new_size;

	// tell the screenshot manager about the new size
	this->screenshot_manager.window_size = new_size;

	// update camgame window position, set it to center.
	this->coord.camgame_window = this->coord.window_size / 2;

	// update camhud window position
	this->coord.camhud_window = {0, (coord::pixel_t) this->coord.window_size.y};

	// reset previous projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// update OpenGL viewport: the renderin area
	glViewport(0, 0, this->coord.window_size.x, this->coord.window_size.y);

	// set orthographic projection: left, right, bottom, top, near_val, far_val
	glOrtho(0, this->coord.window_size.x, 0, this->coord.window_size.y, 9001, -1);

	// reset the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return true;
}

bool Engine::draw_debug_overlay() {
	util::col {255, 255, 255, 255}.use();

	// Draw FPS counter in the lower right corner
	this->render_text(
		{this->coord.window_size.x - 100, 15}, 20,
		"%.1f fps", this->fps_counter.fps
	);

	// Draw version string in the lower left corner
	this->render_text(
		{5, 35}, 20,
		"openage %s", config::version
	);
	this->render_text(
		{5, 15}, 12,
		"%s", config::config_option_string
	);

	this->profiler.show(true);

	return true;
}

void Engine::run() {
	try {
		this->job_manager.start();
		this->running = true;
		this->loop();
		this->running = false;
	} catch (...) {
		this->job_manager.stop();
		throw;
	}
}

void Engine::stop() {
	this->job_manager.stop();
	this->running = false;
}

void Engine::loop() {
	SDL_Event event;
	util::Timer cap_timer;

	while (this->running) {
		this->profiler.start_frame_measure();
		this->fps_counter.frame();
		cap_timer.reset(false);

		this->job_manager.execute_callbacks();

		this->profiler.start_measure("events", {1.0, 0.0, 0.0});
		// top level input handling
		while (SDL_PollEvent(&event)) {
			switch (event.type) {

			case SDL_QUIT:
				this->stop();
				break;

			case SDL_WINDOWEVENT: {
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					coord::window new_size{event.window.data1, event.window.data2};

					// call additional handlers for the resize event
					for (auto &handler : on_resize_handler) {
						if (!handler->on_resize(new_size)) {
							break;
						}
					}
				}
			}

			default:
				for (auto &action : this->on_input_event) {
					if (false == action->on_input(&event)) {
						break;
					}
				}
			} // switch event
		}

		// here, call to Qt and process all the gui events.
		this->gui->process_events();

		if (this->game) {
			// read camera movement input keys, and move camera
			// accordingly.

			// camera movement speed, in pixels per millisecond
			// one pixel per millisecond equals 14.3 tiles/second
			float mov_x = 0.0, mov_y = 0.0, cam_movement_speed_keyboard = 0.5;

			input::InputManager &input = this->get_input_manager();

			if (input.is_down(SDLK_LEFT)) {
				mov_x = -cam_movement_speed_keyboard;
			}
			if (input.is_down(SDLK_RIGHT)) {
				mov_x = cam_movement_speed_keyboard;
			}
			if (input.is_down(SDLK_DOWN)) {
				mov_y = cam_movement_speed_keyboard;
			}
			if (input.is_down(SDLK_UP)) {
				mov_y = -cam_movement_speed_keyboard;
			}

			// perform camera movement
			this->move_phys_camera(
				mov_x, mov_y,
				static_cast<float>(this->lastframe_duration_nsec()) / 1e6);

			// update the currently running game
			this->game->update(this->lastframe_duration_nsec());
		}
		this->profiler.end_measure("events");

		// call engine tick callback methods
		for (auto &action : this->on_engine_tick) {
			if (false == action->on_tick()) {
				break;
			}
		}

		this->profiler.start_measure("rendering", {0.0, 1.0, 0.0});

		// clear the framebuffer to black
		// in the future, we might disable it for lazy drawing
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glPushMatrix(); {
			// set the framebuffer up for camgame rendering
			glTranslatef(coord.camgame_window.x, coord.camgame_window.y, 0);

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

			if (this->drawing_debug_overlay.value) {
				this->draw_debug_overlay();

			}

			if (this->drawing_huds.value) {
				// invoke all hud drawing callback methods
				for (auto &action : this->on_drawhud) {
					if (false == action->on_drawhud()) {
						break;
					}
				}
			}

			this->text_renderer->render();
		}
		glPopMatrix();

		util::gl_check_error();

		this->profiler.end_measure("rendering");

		this->profiler.start_measure("idle", {0.0, 0.0, 1.0});

		// the rendering is done
		// swap the drawing buffers to actually show the frame
		SDL_GL_SwapWindow(window);

		if (this->ns_per_frame != 0) {
			uint64_t ns_for_current_frame = cap_timer.getval();
			if (ns_for_current_frame < this->ns_per_frame) {
				SDL_Delay((this->ns_per_frame - ns_for_current_frame) / 1e6);
			}
		}

		this->profiler.end_measure("idle");

		this->profiler.end_frame_measure();
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
	this->on_resize_handler.push_back(handler);
}

const util::Path &Engine::get_root_dir() {
	return this->root_dir;
}

GameMain *Engine::get_game() {
	return this->game.get();
}

job::JobManager *Engine::get_job_manager() {
	return &this->job_manager;
}

audio::AudioManager &Engine::get_audio_manager() {
	return this->audio_manager;
}

ScreenshotManager &Engine::get_screenshot_manager() {
	return this->screenshot_manager;
}

input::ActionManager &Engine::get_action_manager() {
	return this->action_manager;
}

cvar::CVarManager &Engine::get_cvar_manager() {
	return this->cvar_manager;
}

input::InputManager &Engine::get_input_manager() {
	return this->input_manager;
}

UnitSelection *Engine::get_unit_selection() {
	return this->unit_selection.get();
}

void Engine::announce_global_binds() {
	emit this->gui_signals.global_binds_changed(
		this->get_input_manager().get_global_context().active_binds()
	);
}

renderer::TextRenderer *Engine::get_text_renderer() {
	return this->text_renderer.get();
}

time_nsec_t Engine::lastframe_duration_nsec() const {
	return this->fps_counter.nsec_lastframe;
}

void Engine::render_text(coord::window position, size_t size, const char *format, ...) {
	auto it = this->fonts.find(size);
	if (it == this->fonts.end()) {
		throw Error(MSG(err) << "Unknown font size requested: " << size);
	}

	renderer::Font *font = it->second;

	std::string buf;
	va_list vl;
	va_start(vl, format);
	util::vsformat(format, vl, buf);
	va_end(vl);

	this->text_renderer->set_font(font);
	this->text_renderer->draw(position.x, position.y, buf);
}

void Engine::move_phys_camera(float x, float y, float amount) {
	// move the cam
	coord::vec2f cam_movement {x, y};

	// this factor controls the scroll speed
	cam_movement *= amount;

	// calculate camera position delta from velocity and frame duration
	coord::camgame_delta cam_delta;
	cam_delta.x = cam_movement.x;
	cam_delta.y = - cam_movement.y;

	//update camera phys position
	this->coord.camgame_phys += cam_delta.to_phys3();
}

void Engine::start_game(std::unique_ptr<GameMain> &&game) {
	// TODO: maybe implement a proper 1-to-1 connection
	ENSURE(game, "linking game to engine problem");

	this->game = std::move(game);
	this->game->set_parent(this);
}

void Engine::start_game(const Generator &generator) {
	this->game = std::make_unique<GameMain>(generator);
	this->game->set_parent(this);
}

void Engine::end_game() {
	this->game = nullptr;
	this->unit_selection->clear();
}


} // openage
