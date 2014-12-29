// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#include "game_main.h"

#include <SDL2/SDL.h>
#include "crossplatform/opengl.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cinttypes>

#include "args.h"
#include "audio/sound.h"
#include "callbacks.h"
#include "console/console.h"
#include "coord/vec2f.h"
#include "engine.h"
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
	GameMain test{&engine};
	log::msg("Loading time   [game]: %5.3f s", timer.getval() / 1000.f);

	// run main loop
	engine.run();

	Engine::destroy();

	return 0;
}

GameMain::GameMain(Engine *engine)
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
	this->gamedata_load_job = engine->get_job_manager()->enqueue<std::vector<gamedata::empiresdat>>(gamedata_load_function);
}

void GameMain::on_gamedata_loaded(std::vector<gamedata::empiresdat> &gamedata) {
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
}

GameMain::~GameMain() {
	// oh noes, release hl3 before that!
	delete this->gaben;

	delete this->terrain;

	delete texture_shader::program;
	delete teamcolor_shader::program;
	delete alphamask_shader::program;
}


bool GameMain::on_input(SDL_Event *e) {
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

			// move the cam
			coord::vec2f cam_movement {0.0, 0.0};
			cam_movement.x = e->motion.xrel;
			cam_movement.y = e->motion.yrel;

			// this factor controls the scroll speed
			// cam_movement *= 1;

			// calculate camera position delta from velocity and frame duration
			coord::camgame_delta cam_delta;
			cam_delta.x = cam_movement.x;
			cam_delta.y = - cam_movement.y;

			//update camera phys position
			engine.camgame_phys += cam_delta.to_phys3();
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

void GameMain::move_camera() {
	Engine &engine = Engine::get();
	// read camera movement input keys, and move camera
	// accordingly.

	// camera movement speed, in pixels per millisecond
	// one pixel per millisecond equals 14.3 tiles/second
	float cam_movement_speed_keyboard = 0.5;

	coord::vec2f cam_movement {0.0, 0.0};

	CoreInputHandler &input_handler = engine.get_input_handler();

	if (input_handler.is_key_down(SDLK_LEFT)) {
		cam_movement.x -= cam_movement_speed_keyboard;
	}
	if (input_handler.is_key_down(SDLK_RIGHT)) {
		cam_movement.x += cam_movement_speed_keyboard;
	}
	if (input_handler.is_key_down(SDLK_DOWN)) {
		cam_movement.y -= cam_movement_speed_keyboard;
	}
	if (input_handler.is_key_down(SDLK_UP)) {
		cam_movement.y += cam_movement_speed_keyboard;
	}

	cam_movement *= (float) engine.lastframe_msec();

	// calculate camera position delta from velocity and frame duration
	coord::camgame_delta cam_delta;
	cam_delta.x = cam_movement.x;
	cam_delta.y = cam_movement.y;

	// update camera phys position
	engine.camgame_phys += cam_delta.to_phys3();
}


bool GameMain::on_tick() {
	this->move_camera();
	assetmanager.check_updates();

	if (not gamedata_loaded and this->gamedata_load_job.is_finished()) {
		auto gamedata = this->gamedata_load_job.get_result();
		this->on_gamedata_loaded(gamedata);
		gamedata_loaded = true;
	}
	return true;
}

bool GameMain::on_draw() {
	Engine &engine = Engine::get();

	// draw gaben, our great and holy protector, bringer of the half-life 3.
	gaben->draw(coord::camgame {0, 0});

	// draw terrain
	terrain->draw(&engine);

	if (this->debug_grid_active) {
		this->draw_debug_grid();
	}

	if (not gamedata_loaded) {
		// Show that gamedata is still loading
		engine.dejavuserif20->render(0, 0, "Loading gamedata...");
	}

	return true;
}

bool GameMain::on_drawhud() {
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

Texture *GameMain::find_graphic(int16_t graphic_id) {
	if (graphic_id <= 0 || this->graphics.count(graphic_id) == 0) {
		log::msg("  -> ignoring graphics_id: %d", graphic_id);
		return nullptr;
	}

	int slp_id = this->graphics[graphic_id]->slp_id;
	if (slp_id <= 0) {
		log::msg("  -> ignoring slp_id: %d", slp_id);
		return nullptr;
	}

	log::msg("   slp id/name: %d %s", slp_id, this->graphics[graphic_id]->name0.c_str());
	char *tex_fname = util::format("converted/Data/graphics.drs/%d.slp.png", slp_id);
	Texture *tex = this->assetmanager.get_texture(tex_fname);
	delete[] tex_fname;
	return tex;
}

TestSound *GameMain::find_sound(int16_t sound_id) {
	return &this->available_sounds[sound_id];
}

void GameMain::draw_debug_grid() {
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

} //namespace openage
