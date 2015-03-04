// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "game.h"

#include <SDL2/SDL.h>
#include <cinttypes>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "args.h"
#include "audio/sound.h"
#include "callbacks.h"
#include "console/console.h"
#include "coord/vec2f.h"
#include "crossplatform/opengl.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "crossplatform/opengl.h"
#include <FTGL/ftgl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "callbacks.h"
#include "config.h"
#include "texture.h"
#include "log.h"
#include "util/color.h"
#include "util/error.h"
#include "util/fps.h"
#include "util/opengl.h"
#include "util/strings.h"

#include "gamedata/string_resource.gen.h"
#include "log.h"
#include "terrain/terrain.h"
#include "unit/action.h"
#include "unit/producer.h"
#include "unit/unit.h"
#include "util/strings.h"
#include "util/timer.h"
#include "util/externalprofiler.h"

namespace openage {

/** @file
 * main game logic file.
 *
 * Beware: this file is full of dirty hacks.
 *
 * It's used to test and demonstrate the features of openage.
 * This file will be completely obsolete once the engine is feature-complete.
 *
 * Please try to move out anything located in here and create engine features instead.
 */

/**
* stores all things that have to do with the game.
*
* this includes textures, objects, terrain, etc.
* movement and transformation stuff, and actually everything that
* makes the game work lies in here...
*/

// size of the initial terrain
constexpr coord::tile_delta terrain_data_size = {16, 16};

// terrain ids for the initial terrain
constexpr int terrain_data[16 * 16] = {
	  0,  0,  0,  0,  0,  0,  0,  0, 16,  0,  2,  1, 15, 15, 15,  1,
	  0, 18, 18, 18, 18, 18,  0,  0, 16,  0,  2,  1, 15, 14, 15,  1,
	 18, 18,  0,  0, 18, 18,  0,  0, 16,  0,  2,  1, 15, 15, 15,  1,
	 18, 18,  0,  0, 18, 18,  0,  0, 16,  0,  2,  1,  1,  1,  2,  2,
	 18, 18, 18,  0, 18, 18,  9,  9, 16,  0,  0,  2,  2,  2,  0,  0,
	 18, 18,  0,  0,  0,  0,  9,  9, 16,  0,  0,  0,  0,  0,  0,  0,
	  0, 18,  0,  0,  0,  9,  9,  9, 16,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  2,  0,  9,  9,  0,  2,  2,  0,  0,  0,  0, 23, 23,
	  0,  0,  2, 15,  2,  9,  0,  2, 15, 15,  2,  0,  0,  0,  0,  0,
	  0,  0,  2, 15,  2,  2,  2, 15,  2,  2,  0,  0,  0,  0,  0,  0,
	  0,  0,  2, 15,  2,  2,  2, 15,  2,  0,  0,  0, 20, 20, 20,  0,
	  0,  2,  2, 15,  2,  2,  2, 14,  2,  0,  0,  0, 21, 21, 21,  0,
	  2, 15, 15, 15, 15, 15, 14, 14,  2,  0,  0,  0, 22, 22, 22,  0,
	  0,  2,  2,  2,  2,  2,  2,  2,  0,  0,  0,  0,  5,  5,  5,  0,
	  0,  0,  0,  0,  0,  0,  0,  0, 16, 16, 16, 16,  5,  5,  5,  5,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  5
};


int run_game(Arguments *args) {
	util::Timer timer;

	struct stat data_dir_stat;

	if (stat(args->data_directory, &data_dir_stat) == -1) {
		throw util::Error("Failed checking directory %s: %s",
		                  args->data_directory, strerror(errno));
	}

	log::msg("launching engine with data directory '%s'", args->data_directory);
	util::Dir data_dir{args->data_directory};

	timer.start();
	Engine::create(&data_dir, "openage");
	Engine &engine = Engine::get();

	// initialize terminal colors
	auto termcolors = util::read_csv_file<gamedata::palette_color>(data_dir.join("converted/termcolors.docx"));

	console::Console console(termcolors);
	console.register_to_engine(&engine);

	log::msg("Loading time [engine]: %5.3f s", timer.getval() / 1000.f);

	// init the test run
	timer.start();
	Game test{&engine};
	log::msg("Loading time   [game]: %5.3f s", timer.getval() / 1000.f);

	// run main loop
	engine.run();

	Engine::destroy();

	return 0;
}

Game::Game(Engine *engine)
	:
	editor_current_terrain{0},
	editor_current_building{0},
	debug_grid_active{false},
	clicking_active{true},
	ctrl_active{false},
	scrolling_active{false},
	construct_mode{true},
	selected_unit{nullptr},
	assetmanager{engine->get_data_dir()},
	gamedata_loaded{false},
	engine{engine} {

	engine->register_draw_action(this);
	engine->register_input_action(this);
	engine->register_tick_action(this);
	engine->register_tick_action(&this->placed_units);
	engine->register_drawhud_action(this);

	util::Dir *data_dir = engine->get_data_dir();
	util::Dir asset_dir = data_dir->append("converted");

	// load textures and stuff
	gaben      = new Texture{data_dir->join("gaben.png")};

	auto string_resources = util::read_csv_file<gamedata::string_resource>(asset_dir.join("string_resources.docx"));
	auto terrain_types  = util::read_csv_file<gamedata::terrain_type>(asset_dir.join("gamedata/gamedata-empiresdat/0000-terrains.docx"));
	auto blending_modes = util::read_csv_file<gamedata::blending_mode>(asset_dir.join("blending_modes.docx"));

	// create the terrain which will be filled by chunks
	terrain = new Terrain(assetmanager, terrain_types, blending_modes, true);
	terrain->fill(terrain_data, terrain_data_size);

	auto player_color_lines = util::read_csv_file<gamedata::palette_color>(asset_dir.join("player_palette_50500.docx"));

	GLfloat *playercolors = new GLfloat[player_color_lines.size() * 4];
	for (size_t i = 0; i < player_color_lines.size(); i++) {
		auto line = &player_color_lines[i];
		playercolors[i*4]     = line->r / 255.0;
		playercolors[i*4 + 1] = line->g / 255.0;
		playercolors[i*4 + 2] = line->b / 255.0;
		playercolors[i*4 + 3] = line->a / 255.0;
	}

	// shader initialisation
	// read shader source codes and create shader objects for wrapping them.

	char *texture_vert_code;
	util::read_whole_file(&texture_vert_code, data_dir->join("shaders/maptexture.vert.glsl"));
	auto plaintexture_vert = new shader::Shader(GL_VERTEX_SHADER, texture_vert_code);
	delete[] texture_vert_code;

	char *texture_frag_code;
	util::read_whole_file(&texture_frag_code, data_dir->join("shaders/maptexture.frag.glsl"));
	auto plaintexture_frag = new shader::Shader(GL_FRAGMENT_SHADER, texture_frag_code);
	delete[] texture_frag_code;

	char *teamcolor_frag_code;
	util::read_whole_file(&teamcolor_frag_code, data_dir->join("shaders/teamcolors.frag.glsl"));
	auto teamcolor_frag = new shader::Shader(GL_FRAGMENT_SHADER, teamcolor_frag_code);
	delete[] teamcolor_frag_code;

	char *alphamask_vert_code;
	util::read_whole_file(&alphamask_vert_code, data_dir->join("shaders/alphamask.vert.glsl"));
	auto alphamask_vert = new shader::Shader(GL_VERTEX_SHADER, alphamask_vert_code);
	delete[] alphamask_vert_code;

	char *alphamask_frag_code;
	util::read_whole_file(&alphamask_frag_code, data_dir->join("shaders/alphamask.frag.glsl"));
	auto alphamask_frag = new shader::Shader(GL_FRAGMENT_SHADER, alphamask_frag_code);
	delete[] alphamask_frag_code;



	// create program for rendering simple textures
	texture_shader::program = new shader::Program(plaintexture_vert, plaintexture_frag);
	texture_shader::program->link();
	texture_shader::texture = texture_shader::program->get_uniform_id("texture");
	texture_shader::tex_coord = texture_shader::program->get_attribute_id("tex_coordinates");
	texture_shader::program->use();
	glUniform1i(texture_shader::texture, 0);
	texture_shader::program->stopusing();


	// create program for tinting textures at alpha-marked pixels
	// with team colors
	teamcolor_shader::program = new shader::Program(plaintexture_vert, teamcolor_frag);
	teamcolor_shader::program->link();
	teamcolor_shader::texture = teamcolor_shader::program->get_uniform_id("texture");
	teamcolor_shader::tex_coord = teamcolor_shader::program->get_attribute_id("tex_coordinates");
	teamcolor_shader::player_id_var = teamcolor_shader::program->get_uniform_id("player_number");
	teamcolor_shader::alpha_marker_var = teamcolor_shader::program->get_uniform_id("alpha_marker");
	teamcolor_shader::player_color_var = teamcolor_shader::program->get_uniform_id("player_color");
	teamcolor_shader::program->use();
	glUniform1i(teamcolor_shader::texture, 0);
	glUniform1f(teamcolor_shader::alpha_marker_var, 254.0/255.0);
	// fill the teamcolor shader's player color table:
	glUniform4fv(teamcolor_shader::player_color_var, 64, playercolors);
	teamcolor_shader::program->stopusing();
	delete[] playercolors;


	// create program for drawing textures that are alpha-masked before
	alphamask_shader::program = new shader::Program(alphamask_vert, alphamask_frag);
	alphamask_shader::program->link();
	alphamask_shader::base_coord = alphamask_shader::program->get_attribute_id("base_tex_coordinates");
	alphamask_shader::mask_coord = alphamask_shader::program->get_attribute_id("mask_tex_coordinates");
	alphamask_shader::show_mask = alphamask_shader::program->get_uniform_id("show_mask");
	alphamask_shader::base_texture = alphamask_shader::program->get_uniform_id("base_texture");
	alphamask_shader::mask_texture = alphamask_shader::program->get_uniform_id("mask_texture");
	alphamask_shader::program->use();
	glUniform1i(alphamask_shader::base_texture, 0);
	glUniform1i(alphamask_shader::mask_texture, 1);
	alphamask_shader::program->stopusing();

	// after linking, the shaders are no longer necessary
	delete plaintexture_vert;
	delete plaintexture_frag;
	delete teamcolor_frag;
	delete alphamask_vert;
	delete alphamask_frag;

	auto gamedata_load_function = [this, engine, asset_dir]() -> std::vector<gamedata::empiresdat> {
		log::msg("loading game specification files... stand by, will be faster soon...");
		util::Dir gamedata_dir = asset_dir.append("gamedata");
		return std::move(util::recurse_data_files<gamedata::empiresdat>(gamedata_dir, "gamedata-empiresdat.docx"));
	};
	auto gamedata_load_callback = [this](job::result_function_t<std::vector<gamedata::empiresdat>> get_result) {
		auto result = get_result();
		this->on_gamedata_loaded(result);
	};
	engine->get_job_manager()->enqueue<std::vector<gamedata::empiresdat>>(gamedata_load_function, gamedata_load_callback);
}

void Game::on_gamedata_loaded(std::vector<gamedata::empiresdat> &gamedata) {
	util::Dir *data_dir = this->engine->get_data_dir();
	util::Dir asset_dir = data_dir->append("converted");

	// create graphic id => graphic map
	for (auto &graphic : gamedata[0].graphics.data) {
		this->graphics[graphic.id] = &graphic;
	}

	int your_civ_id = 1; //British by default
	// 0 is gaia and not very useful (it's not an user facing civilization so
	// we cannot rely on it being polished... it might be VERY broken.
	// British or any other civ is a way safer bet.

	log::msg("Using the %s civilisation.", gamedata[0].civs.data[your_civ_id].name.c_str());

	ProducerLoader pload(this);
	available_objects = pload.create_producers(gamedata, your_civ_id);

	auto get_sound_file_location = [asset_dir](int32_t resource_id) -> std::string {
		std::unique_ptr<char[]> snd_file_location;
		// We check in sounds_x1.drs folder first in case we need to override
		for (const char *sound_dir : {"sounds_x1.drs", "sounds.drs"}) {
			snd_file_location.reset(util::format("Data/%s/%d.opus", sound_dir, resource_id));
			if (util::file_size(asset_dir.join(snd_file_location.get())) > 0) {
				return std::move(std::string(snd_file_location.get()));
			}
		}
		// We could not find the sound file for the provided resource_id in both directories
		return "";
	};

	// playable sound files for the audio manager
	std::vector<gamedata::sound_file> sound_files;
	for (gamedata::sound &sound : gamedata[0].sounds.data) {
		std::vector<int> sound_items;

		for (gamedata::sound_item &item : sound.sound_items.data) {
			std::string snd_file_location = get_sound_file_location(item.resource_id);
			if (snd_file_location.empty()) {
				log::msg("   No sound file found for resource_id %d, ignoring...", item.resource_id);
				continue;
			}

			sound_items.push_back(item.resource_id);

			gamedata::sound_file f {
				gamedata::audio_category_t::GAME,
				item.resource_id,
				snd_file_location,
				gamedata::audio_format_t::OPUS,
				gamedata::audio_loader_policy_t::DYNAMIC
			};
			sound_files.push_back(f);
		}
		// create test sound objects that can be played later
		this->available_sounds[sound.id] = TestSound{sound_items};
	}

	// load the requested sounds.
	audio::AudioManager &am = engine->get_audio_manager();
	am.load_resources(asset_dir, sound_files);

	this->gamedata_loaded = true;

}

Game::~Game() {
	// oh noes, release hl3 before that!
	delete this->gaben;

	delete this->terrain;

	delete texture_shader::program;
	delete teamcolor_shader::program;
	delete alphamask_shader::program;
}


bool Game::on_input(SDL_Event *e) {
	Engine &engine = Engine::get();

	switch (e->type) {

	case SDL_QUIT:
		engine.stop();
		break;

	case SDL_MOUSEBUTTONDOWN: { //thanks C++! we need a separate scope because of new variables...

		// a mouse button was pressed...
		// subtract value from window height to get position relative to lower right (0,0).
		coord::window mousepos_window {(coord::pixel_t) e->button.x, (coord::pixel_t) e->button.y};
		coord::camgame mousepos_camgame = mousepos_window.to_camgame();
		// TODO once the terrain elevation milestone is implemented, use a method
		// more suitable for converting camgame to phys3
		coord::phys3 mousepos_phys3 = mousepos_camgame.to_phys3();
		coord::tile mousepos_tile = mousepos_phys3.to_tile3().to_tile();

		if (clicking_active and e->button.button == SDL_BUTTON_LEFT and !construct_mode) {
			TerrainChunk *chunk = terrain->get_chunk(mousepos_tile);
			if (chunk == nullptr) {
				break;
			}

			// get object currently standing at the clicked position
			if (!chunk->get_data(mousepos_tile)->obj.empty()) {
				TerrainObject *obj = chunk->get_data(mousepos_tile)->obj[0];
				selected_unit = obj->unit;
			}
		}
		else if (clicking_active and e->button.button == SDL_BUTTON_LEFT and construct_mode) {
			log::dbg("LMB [window]:    x %9hd y %9hd",
			         mousepos_window.x,
			         mousepos_window.y);
			log::dbg("LMB [camgame]:   x %9hd y %9hd",
			         mousepos_camgame.x,
			         mousepos_camgame.y);

			auto phys_per_tile = openage::coord::settings::phys_per_tile;
			log::dbg("LMB [phys3]:     NE %8.3f SE %8.3f UP %8.3f",
			         ((float) mousepos_phys3.ne) / phys_per_tile,
			         ((float) mousepos_phys3.se) / phys_per_tile,
			         ((float) mousepos_phys3.up) / phys_per_tile);
			log::dbg("LMB [tile]:      NE %8" PRIi64 " SE %8" PRIi64,
			         mousepos_tile.ne,
			         mousepos_tile.se);

			TerrainChunk *chunk = terrain->get_create_chunk(mousepos_tile);
			chunk->get_data(mousepos_tile)->terrain_id = editor_current_terrain;
		}
		else if (clicking_active and e->button.button == SDL_BUTTON_RIGHT and !construct_mode and selected_unit) {
			TerrainChunk *chunk = terrain->get_chunk(mousepos_tile);
			if (chunk == nullptr) {
				break;
			}

			// get object currently standing at the clicked position
			if (chunk->get_data(mousepos_tile)->obj.empty()) {
				selected_unit->target(mousepos_phys3);
			}
			else {
				// try target unit
				Unit *obj = chunk->get_data(mousepos_tile)->obj[0]->unit;
				if (!selected_unit->target(obj)) {
					selected_unit->target(mousepos_phys3);
				}
			}
		}
		else if (clicking_active and e->button.button == SDL_BUTTON_RIGHT and construct_mode) {
			// get chunk clicked on, don't create it if it's not there already
			// -> placing buildings in void is forbidden that way
			TerrainChunk *chunk = terrain->get_chunk(mousepos_tile);
			if (chunk == nullptr) {
				break;
			}

			// delete any unit on the tile
			if (!chunk->get_data(mousepos_tile)->obj.empty()) {
				// get first object currently standing at the clicked position
				TerrainObject *obj = chunk->get_data(mousepos_tile)->obj[0];
				if (obj->unit == selected_unit) selected_unit = nullptr;
				obj->unit->delete_unit();
			} else if ( this->available_objects.size() > 0 ) {
				// try creating a unit
				UnitProducer &producer = *this->available_objects[this->editor_current_building];
				this->placed_units.new_unit(producer, terrain, mousepos_tile);
			}
			break;
		}
		else if (not scrolling_active
		         and e->button.button == SDL_BUTTON_MIDDLE) {
			// activate scrolling
			SDL_SetRelativeMouseMode(SDL_TRUE);
			scrolling_active = true;

			// deactivate clicking as long as mousescrolling is active
			clicking_active = false;
		}
		break;
	}

	case SDL_MOUSEBUTTONUP:
		if (scrolling_active and e->button.button == SDL_BUTTON_MIDDLE) {
			// stop scrolling
			SDL_SetRelativeMouseMode(SDL_FALSE);
			scrolling_active = false;

			// reactivate mouse clicks as scrolling is over
			clicking_active = true;
		}
		break;

	case SDL_MOUSEMOTION:

		// scroll, if middle mouse is being pressed
		//  SDL_GetRelativeMouseMode() queries sdl for that.
		if (scrolling_active) {
			engine.move_phys_camera(e->motion.xrel, e->motion.yrel);
		}
		break;

	case SDL_MOUSEWHEEL:
		if (this->ctrl_active && this->available_objects.size() > 0) {
			editor_current_building = util::mod<ssize_t>(editor_current_building + e->wheel.y, this->available_objects.size());
		}
		else {
			editor_current_terrain = util::mod<ssize_t>(editor_current_terrain + e->wheel.y, this->terrain->terrain_id_count);
		}
		break;

	case SDL_KEYUP:
		switch (((SDL_KeyboardEvent *) e)->keysym.sym) {

		case SDLK_ESCAPE:
			//stop the game
			engine.stop();
			break;

		case SDLK_F1:
			engine.drawing_huds = !engine.drawing_huds;
			break;

		case SDLK_F2:
			engine.get_screenshot_manager().save_screenshot();
			break;

		case SDLK_F3:
			engine.drawing_debug_overlay = !engine.drawing_debug_overlay;
			break;

		case SDLK_F4:
			this->debug_grid_active = !this->debug_grid_active;
			break;

		case SDLK_SPACE:
			this->terrain->blending_enabled = !terrain->blending_enabled;
			break;

		case SDLK_F12:
			if (this->external_profiler.currently_profiling) {
				this->external_profiler.stop();
				this->external_profiler.show_results();
			} else {
				this->external_profiler.start();
			}

			break;

		case SDLK_LCTRL:
			this->ctrl_active = false;
			break;
		case SDLK_m:
			this->construct_mode = !this->construct_mode;
			break;
		case SDLK_p:
			UnitAction::show_debug = !UnitAction::show_debug;
			break;
		}

		break;
	case SDL_KEYDOWN:
		switch (((SDL_KeyboardEvent *) e)->keysym.sym) {
		case SDLK_LCTRL:
			this->ctrl_active = true;
			break;
		}

		break;
	}

	return true;
}

void Game::move_camera() {
	Engine &engine = Engine::get();
	// read camera movement input keys, and move camera
	// accordingly.

	// camera movement speed, in pixels per millisecond
	// one pixel per millisecond equals 14.3 tiles/second
	float mov_x = 0.0, mov_y = 0.0, cam_movement_speed_keyboard = 0.5;

	CoreInputHandler &input_handler = engine.get_input_handler();

	if (input_handler.is_key_down(SDLK_LEFT)) {
		mov_x = -cam_movement_speed_keyboard;
	}
	if (input_handler.is_key_down(SDLK_RIGHT)) {
		mov_x = cam_movement_speed_keyboard;
	}
	if (input_handler.is_key_down(SDLK_DOWN)) {
		mov_y = cam_movement_speed_keyboard;
	}
	if (input_handler.is_key_down(SDLK_UP)) {
		mov_y = -cam_movement_speed_keyboard;
	}

	engine.move_phys_camera(mov_x, mov_y, (float) engine.lastframe_msec());
}


bool Game::on_tick() {
	this->move_camera();
	assetmanager.check_updates();
	return true;
}

bool Game::on_draw() {
	Engine &engine = Engine::get();

	// draw gaben, our great and holy protector, bringer of the half-life 3.
	gaben->draw(coord::camgame{0, 0});

	// draw terrain
	terrain->draw(&engine);

	if (this->debug_grid_active) {
		this->draw_debug_grid();
	}

	if (not gamedata_loaded) {
		// Show that gamedata is still loading
		engine.render_text({0, 0}, 20, "Loading gamedata...");
	}

	// draw construction or actions mode indicator
	int x = 400 - (engine.window_size.x / 2);
	int y = 35 - (engine.window_size.y / 2);
	if (construct_mode) {
		engine.render_text({x, y}, 20, "Construct mode");
	} else {
		engine.render_text({x, y}, 20, "Actions mode");
	}

	return true;
}

bool Game::on_drawhud() {
	Engine &e = Engine::get();

	// draw the currently selected editor texture tile
	this->terrain->texture(this->editor_current_terrain)->draw(coord::window{63, 84}.to_camhud(), ALPHAMASKED);

	if (this->available_objects.size() > 0) {
		// and the current active building
		coord::window bpreview_pos;
		bpreview_pos.x = e.window_size.x - 200;
		bpreview_pos.y = 200;
		auto txt = this->available_objects[this->editor_current_building].get()->default_texture();
		txt->draw(bpreview_pos.to_camhud());
	}
	return true;
}

Texture *Game::find_graphic(int graphic_id) {
	auto tex_it = this->graphics.find(graphic_id);
	if (tex_it == this->graphics.end()) {
		log::msg("  -> ignoring graphics_id: %d", graphic_id);
		return nullptr;
	}

	int slp_id = tex_it->second->slp_id;
	if (slp_id <= 0) {
		log::msg("  -> ignoring slp_id: %d", slp_id);
		return nullptr;
	}

	log::msg("   slp id/name: %d %s", slp_id, this->graphics[graphic_id]->name0.c_str());
	std::string tex_fname = util::sformat("converted/Data/graphics.drs/%d.slp.png", slp_id);
	return this->assetmanager.get_texture(tex_fname);
}

TestSound *Game::find_sound(int sound_id) {
	return &this->available_sounds[sound_id];
}

void Game::draw_debug_grid() {
	Engine &e = Engine::get();

	coord::camgame camera = coord::tile{0, 0}.to_tile3().to_phys3().to_camgame();

	int cam_offset_x = util::mod(camera.x, e.tile_halfsize.x * 2);
	int cam_offset_y = util::mod(camera.y, e.tile_halfsize.y * 2);

	int line_half_width = e.window_size.x / 2;
	int line_half_height = e.window_size.y / 2;

	// rounding so we get 2:1 proportion needed for the isometric perspective

	if (line_half_width > line_half_height * 2) {

		if (line_half_width & 1) {
			line_half_width += 1; // round up if it's odd
		}

		line_half_height = line_half_width / 2;

	} else {
		line_half_width = line_half_height * 2;
	}

	// quantity of lines to draw to each side from the center
	int k = line_half_width / (e.tile_halfsize.x);

	int tilesize_x = e.tile_halfsize.x * 2;
	int common_x   = cam_offset_x + e.tile_halfsize.x;
	int x0         = common_x     - line_half_width;
	int x1         = common_x     + line_half_width;
	int y0         = cam_offset_y - line_half_height;
	int y1         = cam_offset_y + line_half_height;

	glLineWidth(1);
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES); {

		for (int i = -k; i < k; i++) {
				glVertex3f(i * tilesize_x + x0, y1, 0);
				glVertex3f(i * tilesize_x + x1, y0, 0);

				glVertex3f(i * tilesize_x + x0, y0 - 1, 0);
				glVertex3f(i * tilesize_x + x1, y1 - 1, 0);
		}

	} glEnd();

}

void TestSound::play() {
	if (this->sound_items.size() <= 0) {
		return;
	}
	audio::AudioManager &am = Engine::get().get_audio_manager();

	int rand = util::random_range(0, this->sound_items.size());
	int sndid = this->sound_items[rand];
	try {
		audio::Sound{am.get_sound(audio::category_t::GAME, sndid)}.play();
	}
	catch(util::Error &e) {
		log::dbg("cannot play: %s", e.str());
	}
}

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
running{false},
drawing_debug_overlay{true},
drawing_huds{true},
window_size{800, 600},
camgame_phys{10 * coord::settings::phys_per_tile, 10 * coord::settings::phys_per_tile, 0},
camgame_window{400, 300},
camhud_window{0, 600},
tile_halfsize{48, 24},  // TODO: get from convert script
data_dir{data_dir},
audio_manager{} {

	for (uint32_t size : {12, 20}) {
		fonts[size] = std::unique_ptr<Font>{new Font{"DejaVu Serif", "Book", size}};
	}

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

	int max_texture_units;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_texture_units);
	log::dbg("Maximum supported texture units: %d", max_texture_units);
	if (max_texture_units < 2) {
		throw util::Error("Your GPU has too less texture units: %d", max_texture_units);
	}

	// vsync on
	SDL_GL_SetSwapInterval(1);

	// enable alpha blending
	glEnable(GL_BLEND);

	// order of drawing relevant for depth
	// what gets drawn last is displayed on top.
	glDisable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	// tell the screenshot manager about the new size
	this->screenshot_manager.window_size = new_size;

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

bool Engine::draw_debug_overlay() {
	util::col {255, 255, 255, 255}.use();

	// Draw FPS counter in the lower right corner
	this->render_text(
{this->window_size.x - 100, 15}, 20,
"%.1f fps", this->fpscounter.fps
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

return true;
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

		this->job_manager->execute_callbacks();

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

			if (this->drawing_debug_overlay) {
				this->draw_debug_overlay();
			}

			if (this->drawing_huds) {
				// invoke all hud drawing callback methods
				for (auto &action : this->on_drawhud) {
					if (false == action->on_drawhud()) {
						break;
					}
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

ScreenshotManager &Engine::get_screenshot_manager() {
	return this->screenshot_manager;
}

CoreInputHandler &Engine::get_input_handler() {
	return this->input_handler;
}

unsigned int Engine::lastframe_msec() {
	return this->fpscounter.msec_lastframe;
}

void Engine::render_text(coord::window position, size_t size, const char *format, ...) {
	auto it = this->fonts.find(size);
	if (it == this->fonts.end()) {
		throw util::Error("unknown font size %zu requested.", size);
	}

	Font *font = it->second.get();

	va_list vl;
	va_start(vl, format);
	std::string buf = util::vsformat(format, vl);
	va_end(vl);

	font->render_static(position.x, position.y, buf.c_str());
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
	this->camgame_phys += cam_delta.to_phys3();
}

} //namespace openage
