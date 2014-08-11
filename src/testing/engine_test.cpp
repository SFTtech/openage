#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../args.h"
#include "../audio/sound.h"
#include "../callbacks.h"
#include "../console/console.h"
#include "../coord/vec2f.h"
#include "../engine.h"
#include "../gamedata/string_resource.h"
#include "../gamedata/gamedata.h"
#include "../log.h"
#include "../terrain.h"
#include "../util/timer.h"

#include "engine_test.h"

namespace testing {

using namespace openage;

/*
  beware: this file is full of dirty hacks.

  it's used to test and demonstrate the features of openage.
  this file will be completely obsolete once the engine is feature-complete.
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
	  0, 18,  0,  0,  0,  9,  9,  9, 16,  0,  0, -1,  0,  0,  0,  0,
	  0,  0,  0,  2,  0,  9,  9,  0,  2,  2,  0,  0, -1,  0, 23, 23,
	  0,  0,  2, 15,  2,  9,  0,  2, 15, 15,  2,  0, -1,  0,  0,  0,
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
	auto termcolors = util::read_csv_file<gamedata::palette_color>(data_dir.join("age/assets/termcolors.docx"));

	console::Console console(termcolors);
	console.register_to_engine(&engine);

	log::msg("Loading time [engine]: %5.3f s", timer.getval() / 1000.f);

	// init the test run
	timer.start();
	EngineTest test{&engine};
	log::msg("Loading time   [game]: %5.3f s", timer.getval() / 1000.f);

	// run main loop
	engine.run();

	return 0;
}

EngineTest::EngineTest(Engine *engine)
	:
	editor_current_terrain(0),
	clicking_active(true),
	scrolling_active(false) {

	engine->register_draw_action(this);
	engine->register_input_action(this);
	engine->register_tick_action(this);
	engine->register_drawhud_action(this);

	util::Dir *data_dir = engine->get_data_dir();
	util::Dir asset_dir = data_dir->append("age/assets");

	// load textures and stuff
	gaben      = new Texture{data_dir->join("gaben.png")};
	university = new Texture{asset_dir.join("Data/graphics.drs/3836.slp.png"), true, PLAYERCOLORED};

	auto string_resources = util::read_csv_file<gamedata::string_resource>(asset_dir.join("string_resources.docx"));
	auto terrain_types  = util::read_csv_file<gamedata::terrain_type>(asset_dir.join("gamedata/gamedata-empiresdat/0000-terrains.docx"));
	auto blending_modes = util::read_csv_file<gamedata::blending_mode>(asset_dir.join("blending_modes.docx"));

	// create the terrain which will be filled by chunks
	terrain = new Terrain(asset_dir, terrain_types, blending_modes, true);
	terrain->fill(terrain_data, terrain_data_size);

	util::Dir gamedata_dir = asset_dir.append("gamedata");
	auto gamedata = util::recurse_data_files<gamedata::empiresdat>(gamedata_dir, "gamedata-empiresdat.docx");

	int i = 0;
	for (auto &civ : gamedata[0].civs.data) {
		int j = 0;
		log::msg("civ[%d]: name = %s, %lu buldings", i, civ.name.c_str(), civ.units.building.data.size());

		for (auto &building : civ.units.building.data) {
			log::msg("  building[%d]: name = %s", j, building.name.c_str());
			j += 1;
		}

		i += 1;
	}

	auto player_color_lines = util::read_csv_file<gamedata::palette_color>(asset_dir.join("player_palette_50500.docx"));

	GLfloat *playercolors = new GLfloat[player_color_lines.size() * 4];
	for (size_t i = 0; i < player_color_lines.size(); i++) {
		auto line = &player_color_lines[i];
		playercolors[i*4]     = line->r / 255.0;
		playercolors[i*4 + 1] = line->g / 255.0;
		playercolors[i*4 + 2] = line->b / 255.0;
		playercolors[i*4 + 3] = line->a / 255.0;
	}

	audio::AudioManager &am = engine->get_audio_manager();

	// initialize sounds
	build_uni_sound    = new audio::Sound{am.get_sound(audio::category_t::GAME, 5229)};
	destroy_uni_sound0 = new audio::Sound{am.get_sound(audio::category_t::GAME, 5316)};
	destroy_uni_sound1 = new audio::Sound{am.get_sound(audio::category_t::GAME, 5317)};

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
}

EngineTest::~EngineTest() {
	for (auto &obj : buildings) {
		delete obj;
	}

	// oh noes, release hl3 before that!
	delete gaben;

	delete terrain;

	delete build_uni_sound;
	delete destroy_uni_sound0;
	delete destroy_uni_sound1;

	delete university;
	delete texture_shader::program;
	delete teamcolor_shader::program;
	delete alphamask_shader::program;
}


bool EngineTest::on_input(SDL_Event *e) {
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

		if (clicking_active and e->button.button == SDL_BUTTON_LEFT) {
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
			log::dbg("LMB [tile]:      NE %8ld SE %8ld",
			         mousepos_tile.ne,
			         mousepos_tile.se);

			TerrainChunk *chunk = terrain->get_create_chunk(mousepos_tile);
			chunk->get_data(mousepos_tile)->terrain_id = editor_current_terrain;
		}
		else if (clicking_active and e->button.button == SDL_BUTTON_RIGHT) {

			// get chunk clicked on, don't create it if it's not there already
			TerrainChunk *chunk = terrain->get_chunk(mousepos_tile);
			if (chunk == nullptr) {
				break;
			}

			// get object currently standing at the clicked position
			TerrainObject *obj = chunk->get_data(mousepos_tile)->obj;
			if (obj != nullptr) {
				obj->remove();
				buildings.erase(obj);
				delete obj;

				// play uni destruction sound
				int rand = util::random_range(0, 1 + 1);
				if (rand == 0) {
					destroy_uni_sound0->play();
				} else {
					destroy_uni_sound1->play();
				}
			} else {
				int uni_player_id = util::random_range(1, 8 + 1);
				TerrainObject *newuni = new TerrainObject(university, uni_player_id);

				log::dbg("uni player id: %d", uni_player_id);

				// try to place the uni, it knows best whether it will fit.
				bool uni_placed = newuni->place(terrain, mousepos_tile);
				if (uni_placed) {
					newuni->set_ground(editor_current_terrain, 0);
					buildings.insert(newuni);
					build_uni_sound->play();
				} else {
					delete newuni;
				}
			}
			break;
		}
		else if (not scrolling_active and e->button.button == SDL_BUTTON_MIDDLE) {
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
		editor_current_terrain = util::mod<ssize_t>(editor_current_terrain + e->wheel.y, terrain->terrain_id_count);
		break;

	case SDL_KEYUP:
		switch (((SDL_KeyboardEvent *) e)->keysym.sym) {
		case SDLK_ESCAPE:
			//stop the game
			engine.stop();
			break;
		}

		break;
	case SDL_KEYDOWN:
		switch (((SDL_KeyboardEvent *) e)->keysym.sym) {
		case SDLK_SPACE:
			terrain->blending_enabled = !terrain->blending_enabled;
			break;
		case SDLK_F2:
			engine.save_screenshot("/tmp/openage_00.png");
			break;
		}

		break;
	}

	return true;
}

void EngineTest::move_camera() {
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


bool EngineTest::on_tick() {
	this->move_camera();

	return true;
}

bool EngineTest::on_draw() {
	Engine &engine = Engine::get();

	// draw gaben, our great and holy protector, bringer of the half-life 3.
	gaben->draw(coord::camgame {0, 0});

	// draw terrain
	terrain->draw(&engine);

	return true;
}

bool EngineTest::on_drawhud() {
	// draw the currently selected editor texture tile
	terrain->texture(editor_current_terrain)->draw(coord::window{63, 84}.to_camhud());

	return true;
}

} //namespace openage
