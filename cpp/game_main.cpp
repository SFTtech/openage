// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "game_main.h"

#include <epoxy/gl.h>
#include <SDL2/SDL.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "args.h"
#include "audio/sound.h"
#include "callbacks.h"
#include "console/console.h"
#include "coord/vec2f.h"
#include "engine.h"
#include "gamedata/string_resource.gen.h"
#include "game_save.h"
#include "keybinds/keybind_manager.h"
#include "log/log.h"
#include "terrain/terrain.h"
#include "unit/action.h"
#include "unit/command.h"
#include "unit/producer.h"
#include "unit/unit.h"
#include "unit/unit_texture.h"
#include "util/strings.h"
#include "util/timer.h"
#include "util/externalprofiler.h"

#include "gui/container.h"
#include "gui/drawer.h"
#include "gui/label.h"
#include "gui/toplevel.h"
#include "gui/imagebutton.h"
#include "gui/imagecheckbox.h"
#include "gui/style.h"
#include "gui/formlayout.h"

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
		throw util::Error(MSG(err) <<
			"Failed checking directory " << args->data_directory <<
			": " << strerror(errno));
	}

	log::log(MSG(info) << "launching engine with data directory '" << args->data_directory << "'");

	util::Dir data_dir{args->data_directory};

	timer.start();
	Engine::create(&data_dir, "openage");
	Engine &engine = Engine::get();

	// initialize terminal colors
	auto termcolors = util::read_csv_file<gamedata::palette_color>(data_dir.join("converted/termcolors.docx"));

	console::Console console;
	console.load_colors(termcolors);
	console.register_to_engine(&engine);

	log::log(MSG(info).fmt("Loading time [engine]: %5.3f s", timer.getval() / 1.0e9));

	// init the test run
	timer.start();
	GameMain test{&engine};

	log::log(MSG(info).fmt("Loading time   [game]: %5.3f s", timer.getval() / 1.0e9));

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
	scrolling_active{false},
	dragging_active{false},
	construct_mode{true},
	building_placement{false},
	use_set_ability{false},
	assetmanager{engine->get_data_dir()},
	engine{engine} {

	// prepare data loading
	datamanager.initialize(&assetmanager);

	// engine callbacks
	this->engine->register_draw_action(this);
	this->engine->register_input_action(this);
	this->engine->register_tick_action(this);
	this->engine->register_tick_action(&this->placed_units);
	this->engine->register_drawhud_action(this);
	this->engine->register_drawhud_action(&this->selection);

	util::Dir *data_dir = engine->get_data_dir();
	util::Dir asset_dir = data_dir->append("converted");

	// load textures and stuff
	gaben      = new Texture{data_dir->join("gaben.png")};

	auto string_resources = util::read_csv_file<gamedata::string_resource>(asset_dir.join("string_resources.docx"));
	auto terrain_types  = util::read_csv_file<gamedata::terrain_type>(asset_dir.join("gamedata/gamedata-empiresdat/0000-terrains.docx"));
	auto blending_modes = util::read_csv_file<gamedata::blending_mode>(asset_dir.join("blending_modes.docx"));

	// create the terrain which will be filled by chunks
	this->terrain = std::make_shared<Terrain>(assetmanager, terrain_types, blending_modes, true);
	this->terrain->fill(terrain_data, terrain_data_size);
	this->placed_units.set_terrain(this->terrain);

	// players
	unsigned int number_of_players = 8;
	for (unsigned int i = 0; i < number_of_players; ++i) {
		this->players.emplace_back(i);
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

	// initialize global keybinds
	auto &global_keybind_context = engine->get_keybind_manager().get_global_keybind_context();

	global_keybind_context.bind(keybinds::action_t::STOP_GAME, [this]() {
		this->engine->stop();
	});
	global_keybind_context.bind(keybinds::action_t::TOGGLE_HUD, [this]() {
		this->engine->drawing_huds = !this->engine->drawing_huds;
	});
	global_keybind_context.bind(keybinds::action_t::SCREENSHOT, [this]() {
		this->engine->get_screenshot_manager().save_screenshot();
	});
	global_keybind_context.bind(keybinds::action_t::TOGGLE_DEBUG_OVERLAY, [this]() {
		this->engine->drawing_debug_overlay = !this->engine->drawing_debug_overlay;
	});
	global_keybind_context.bind(keybinds::action_t::TOGGLE_DEBUG_GRID, [this]() {
		this->debug_grid_active = !this->debug_grid_active;
	});
	global_keybind_context.bind(keybinds::action_t::QUICK_SAVE, [this]() {
		gameio::save(this, "default_save.txt");
	});
	global_keybind_context.bind(keybinds::action_t::QUICK_LOAD, [this]() {
		gameio::load(this, "default_save.txt");
	});
	global_keybind_context.bind(keybinds::action_t::TOGGLE_PROFILER, [this]() {
		if (this->external_profiler.currently_profiling) {
			this->external_profiler.stop();
			this->external_profiler.show_results();
		} else {
			this->external_profiler.start();
		}
	});

	// Local keybinds
	this->keybind_context.bind(keybinds::action_t::TOGGLE_BLENDING, [this]() {
		this->terrain->blending_enabled = !terrain->blending_enabled;
	});
	this->keybind_context.bind(keybinds::action_t::TOGGLE_CONSTRUCT_MODE, [this]() {
		this->construct_mode = !this->construct_mode;
	});
	this->keybind_context.bind(keybinds::action_t::TOGGLE_UNIT_DEBUG, [this]() {
		UnitAction::show_debug = !UnitAction::show_debug;
	});
	this->keybind_context.bind(keybinds::action_t::TRAIN_OBJECT, [this]() {
		// attempt to train editor selected object
		if ( this->datamanager.producer_count() > 0 ) {
			auto type = this->datamanager.get_type_index(this->editor_current_building);
			Command cmd(this->players[this->engine->current_player - 1], type);
			this->selection.all_invoke(cmd);
		}
	});
	this->keybind_context.bind(keybinds::action_t::ENABLE_BUILDING_PLACEMENT, [this]() {
		this->building_placement = true;
	});
	this->keybind_context.bind(keybinds::action_t::DISABLE_SET_ABILITY, [this]() {
		this->use_set_ability = false;
	});
	this->keybind_context.bind(keybinds::action_t::SET_ABILITY_MOVE, [this]() {
		this->use_set_ability = true;
		this->ability = ability_type::move;
	});
	this->keybind_context.bind(keybinds::action_t::SET_ABILITY_GATHER, [this]() {
		this->use_set_ability = true;
		this->ability = ability_type::gather;
	});
	this->keybind_context.bind(keybinds::action_t::SPAWN_VILLAGER, [this]() {
		if (this->construct_mode && this->datamanager.producer_count() > 0) {
			UnitType &type = *this->datamanager.get_type(590);
			this->placed_units.new_unit(type, this->players[this->engine->current_player - 1], mousepos_tile.to_phys2().to_phys3());
		}
	});
	this->keybind_context.bind(keybinds::action_t::KILL_UNIT, [this]() {
		selection.kill_unit();
	});

	// Villager build commands
	// TODO place this into separate building menus instead of global hotkeys
	auto bind_building_key = [this](keybinds::action_t action, int building, int military_building) {
		this->keybind_context.bind(action, [this, building, military_building]() {
			if (this->selection.contains_builders()) {
				this->building_placement = true;
				if (this->engine->get_keybind_manager().is_keymod_down(KMOD_LCTRL)) {
					this->editor_current_building = military_building;
				} else {
					this->editor_current_building = building;
				}
			}
		});
	};
	bind_building_key(keybinds::action_t::BUILDING_1, 598, 609); // House, barracks
	bind_building_key(keybinds::action_t::BUILDING_2, 574, 558); // Mill, archery range
	bind_building_key(keybinds::action_t::BUILDING_3, 616, 581); // Mining camp, stable
	bind_building_key(keybinds::action_t::BUILDING_4, 611, 580); // Lumber camp, siege workshop
	bind_building_key(keybinds::action_t::BUILDING_TOWN_CENTER, 568, 568); // Town center

	// Switching between players with the 1-8 keys
	auto bind_player_switch = [this](keybinds::action_t action, int player) {
		this->keybind_context.bind(action, [this, player]() {
			this->engine->current_player = player;
			this->selection.clear();
		});
	};
	bind_player_switch(keybinds::action_t::SWITCH_TO_PLAYER_1, 1);
	bind_player_switch(keybinds::action_t::SWITCH_TO_PLAYER_2, 2);
	bind_player_switch(keybinds::action_t::SWITCH_TO_PLAYER_3, 3);
	bind_player_switch(keybinds::action_t::SWITCH_TO_PLAYER_4, 4);
	bind_player_switch(keybinds::action_t::SWITCH_TO_PLAYER_5, 5);
	bind_player_switch(keybinds::action_t::SWITCH_TO_PLAYER_6, 6);
	bind_player_switch(keybinds::action_t::SWITCH_TO_PLAYER_7, 7);
	bind_player_switch(keybinds::action_t::SWITCH_TO_PLAYER_8, 8);

	engine->get_keybind_manager().register_context(&this->keybind_context);

	// create GUI
	gui_top_level.reset(new gui::TopLevel);
	gui_top_level->set_layout(std::make_unique<gui::FormLayout>());

	gui::Style style{&assetmanager};

	gui_loading_message = gui_top_level->create<gui::Label>();
	gui_loading_message->set_text("Loading gamedata...");
	gui_loading_message->set_layout_data(gui::FormLayoutData::best_size(gui_loading_message)
			.set_left(gui::FormAttachment::center())
			.set_top(gui::FormAttachment::center()));

	gui_framerate = gui_top_level->create<gui::Label>();
	gui_framerate->set_text("⸘FPS here maybe‽");
	gui_framerate->set_layout_data(gui::FormLayoutData::best_size(gui_framerate)
			.set_left(gui::FormAttachment::right().offset_by(-100))
			.set_bottom(gui::FormAttachment::bottom().offset_by(-30)));

	auto gui_idle_villager = gui_top_level->add_control(style.create_image_button("converted/Data/interfac.drs/50788.slp.png", 12));
	gui_idle_villager->set_layout_data(gui::FormLayoutData::best_size(gui_idle_villager)
			.set_left(gui::FormAttachment::left().offset_by(5))
			.set_bottom(gui::FormAttachment::bottom().offset_by(-5)));
	gui_idle_villager->on_click([]{
		log::log(MSG(info).fmt("Idle villager, anyone?"));
	});

#if 0
	auto gui_flare = gui_top_level->add_control(style.create_image_button("converted/Data/interfac.drs/50788.slp.png", 2));
	gui_flare->set_right(gui::FormAttachment::left().offset_by(90)); // tmp
	gui_flare->set_top(gui::FormAttachment::bottom().offset_by(-85)); // tmp
	gui_flare->set_left(gui::FormAttachment::adjacent_to(gui_idle_villager).offset_by(10));
	gui_flare->set_bottom(gui::FormAttachment::bottom().offset_by(-50));
	gui_flare->on_click([]{
		log::log(MSG(info).fmt("Send a flare!"));
	});

	auto gui_quit = gui_top_level->add_control(style.create_button());
	gui_quit->set_left(gui::FormAttachment::right().offset_by(-42));
	gui_quit->set_right(gui::FormAttachment::right().offset_by(-10));
	gui_quit->set_top(gui::FormAttachment::top().offset_by(10));
	gui_quit->set_bottom(gui::FormAttachment::top().offset_by(40));
	gui_quit->get_label()->set_text("X");
	gui_quit->on_click([engine]{
		engine->stop();
	});

	auto gui_palalol = gui_top_level->add_control(style.create_image_button("converted/Data/interfac.drs/50730.slp.png", 2));
	gui_palalol->set_left(gui::FormAttachment::left().offset_by(10));
	gui_palalol->set_right(gui::FormAttachment::left().offset_by(50));
	gui_palalol->set_top(gui::FormAttachment::center().offset_by(10));
	gui_palalol->set_bottom(gui::FormAttachment::center().offset_by(50));
#endif
}

GameMain::~GameMain() {
	// oh noes, release hl3 before that!
	delete this->gaben;

	delete texture_shader::program;
	delete teamcolor_shader::program;
	delete alphamask_shader::program;
}


bool GameMain::on_input(SDL_Event *e) {
	Engine &engine = Engine::get();
	
	// first, see if the gui wants to process the event
	if (gui_top_level->process_event(e)) {
		return true;
	}

	switch (e->type) {

	case SDL_QUIT:
		engine.stop();
		break;

	case SDL_MOUSEBUTTONDOWN: {
		switch (e->button.button) {
		case SDL_BUTTON_LEFT:
			if (this->clicking_active && !construct_mode && !this->building_placement) {
				// begin a boxed selection
				selection.drag_begin(mousepos_camgame);
				dragging_active = true;
			}
			break;
		case SDL_BUTTON_MIDDLE:
			// activate scrolling
			SDL_SetRelativeMouseMode(SDL_TRUE);
			scrolling_active = true;

			// deactivate clicking as long as mousescrolling is active
			clicking_active = false;
			break;
		}
		break;

	}

	case SDL_MOUSEBUTTONUP: {
		// subtract value from window height to get position relative to lower right (0,0).
		coord::window mousepos_window {(coord::pixel_t) e->button.x, (coord::pixel_t) e->button.y};
		this->mousepos_camgame = mousepos_window.to_camgame();
		// TODO once the terrain elevation milestone is implemented, use a method
		// more suitable for converting camgame to phys3
		this->mousepos_phys3 = mousepos_camgame.to_phys3();
		this->mousepos_tile = mousepos_phys3.to_tile3().to_tile();

		switch (e->button.button) {

		case SDL_BUTTON_LEFT:
			if (this->dragging_active) { // Stop dragging
				selection.drag_release(terrain.get(), this->engine->get_keybind_manager().is_keymod_down(KMOD_LCTRL));
				dragging_active = false;
			} else if (clicking_active) {
				if (construct_mode) {
					log::log(MSG(dbg) <<
					    "LMB [window]:   "
					    " x " << std::setw(9) << mousepos_window.x <<
					    " y " << std::setw(9) << mousepos_window.y);

					constexpr auto phys_per_tile = openage::coord::settings::phys_per_tile;

					log::log(MSG(dbg) <<
					    "LMB [phys3]:    "
					    " NE " << util::FixedPoint<phys_per_tile, 3, 8>{mousepos_phys3.ne} <<
					    " SE " << util::FixedPoint<phys_per_tile, 3, 8>{mousepos_phys3.se} <<
					    " UP " << util::FixedPoint<phys_per_tile, 3, 8>{mousepos_phys3.up});

					log::log(MSG(dbg) <<
					    "LMB [tile]:     "
					    " NE " << std::setw(8) << mousepos_tile.ne <<
					    " SE " << std::setw(8) << mousepos_tile.se);

					TerrainChunk *chunk = terrain->get_create_chunk(mousepos_tile);
					chunk->get_data(mousepos_tile)->terrain_id = editor_current_terrain;
				} else if (this->building_placement) {
					// confirm building placement with left click
					// first create foundation using the producer
					UnitContainer *container = &this->placed_units;
					UnitType *building_type = this->datamanager.get_type_index(this->editor_current_building);
					UnitReference new_building = container->new_unit(*building_type, this->players[engine.current_player - 1], mousepos_phys3);

					// task all selected villagers to build
					if (new_building.is_valid()) {
						Command cmd(this->players[engine.current_player - 1], new_building.get());
						cmd.set_ability(ability_type::build);
						this->selection.all_invoke(cmd);
					}
					this->building_placement = false;
				}
			}
			break;

		case SDL_BUTTON_MIDDLE:
			if (scrolling_active) { // Stop scrolling
				SDL_SetRelativeMouseMode(SDL_FALSE);
				scrolling_active = false;

				// reactivate mouse clicks as scrolling is over
				clicking_active = true;
			}
			break;

		case SDL_BUTTON_RIGHT:
			if (clicking_active) {
				if (construct_mode) {
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
						log::log(MSG(dbg) << "delete unit with unit id " << obj->unit.id);
						obj->unit.delete_unit();
					} else if ( this->datamanager.producer_count() > 0 ) {
						// try creating a unit
						log::log(MSG(dbg) << "create unit with producer id " << this->editor_current_building);
						UnitType &type = *this->datamanager.get_type_index(this->editor_current_building);
						this->placed_units.new_unit(type, this->players[engine.current_player - 1], mousepos_tile.to_phys2().to_phys3());
					}
				} else {
					// right click can cancel building placement
					if (this->building_placement) {
						this->building_placement = false;
					} else {
						auto cmd = this->get_action(mousepos_phys3);
						selection.all_invoke(cmd);
					}
				}
			}
			break;

		} // switch (e->button.button)
		break;

	} // case SDL_MOUSEBUTTONUP:

	case SDL_MOUSEMOTION: {

		// update mouse position values
		coord::window mousepos_window {(coord::pixel_t) e->button.x, (coord::pixel_t) e->button.y};
		this->mousepos_camgame = mousepos_window.to_camgame();
		this->mousepos_phys3 = mousepos_camgame.to_phys3();
		this->mousepos_tile = mousepos_phys3.to_tile3().to_tile();

		if (dragging_active) {
			selection.drag_update(mousepos_camgame);
		}

		// scroll, if middle mouse is being pressed
		//  SDL_GetRelativeMouseMode() queries sdl for that.
		else if (scrolling_active) {
			engine.move_phys_camera(e->motion.xrel, e->motion.yrel);
		}
		break;
	}

	case SDL_MOUSEWHEEL:
		if (this->construct_mode) {
			if (engine.get_keybind_manager().is_keymod_down(KMOD_LCTRL) && this->datamanager.producer_count() > 0) {
				editor_current_building = util::mod<ssize_t>(editor_current_building + e->wheel.y, this->datamanager.producer_count());
			} else {
				editor_current_terrain = util::mod<ssize_t>(editor_current_terrain + e->wheel.y, this->terrain->terrain_id_count);
			}
		}
		break;

	case SDL_KEYUP: {
		SDL_Keymod keymod = SDL_GetModState();

		SDL_Keycode sym = reinterpret_cast<SDL_KeyboardEvent *>(e)->keysym.sym;
		keybinds::KeybindManager &keybinds = engine.get_keybind_manager();
		keybinds.set_key_state(sym, keymod, false);
		keybinds.press(keybinds::key_t(sym, keymod));
		break;
	}

	case SDL_KEYDOWN: {
		SDL_Keycode sym = reinterpret_cast<SDL_KeyboardEvent *>(e)->keysym.sym;
		engine.get_keybind_manager().set_key_state(sym, SDL_GetModState(), true);
		break;
	}


	} // switch (e->type)

	return true;
}

void GameMain::move_camera() {
	Engine &engine = Engine::get();
	// read camera movement input keys, and move camera
	// accordingly.

	// camera movement speed, in pixels per millisecond
	// one pixel per millisecond equals 14.3 tiles/second
	float mov_x = 0.0, mov_y = 0.0, cam_movement_speed_keyboard = 0.5;

	keybinds::KeybindManager &keybinds = engine.get_keybind_manager();

	if (keybinds.is_key_down(SDLK_LEFT)) {
		mov_x = -cam_movement_speed_keyboard;
	}
	if (keybinds.is_key_down(SDLK_RIGHT)) {
		mov_x = cam_movement_speed_keyboard;
	}
	if (keybinds.is_key_down(SDLK_DOWN)) {
		mov_y = cam_movement_speed_keyboard;
	}
	if (keybinds.is_key_down(SDLK_UP)) {
		mov_y = -cam_movement_speed_keyboard;
	}

	engine.move_phys_camera(mov_x, mov_y, (float) engine.lastframe_duration_nsec() / 1e6);
}


bool GameMain::on_tick() {
	this->move_camera();
	assetmanager.check_updates();
	datamanager.check_updates();
	return true;
}

bool GameMain::on_draw() {
	Engine &engine = Engine::get();

	// draw gaben, our great and holy protector, bringer of the half-life 3.
	gaben->draw(coord::camgame{0, 0});

	// draw terrain
	terrain->draw(&engine);

	if (this->debug_grid_active) {
		this->draw_debug_grid();
	}

	if (not this->datamanager.load_complete()) {
		// Show that gamedata is still loading
		engine.render_text({0, 0}, 20, "Loading gamedata...");
	}

	// draw construction or actions mode indicator
	int x = 400 - (engine.engine_coord_data->window_size.x / 2);
	int y = 35 - (engine.engine_coord_data->window_size.y / 2);

	std::string mode_str;
	if (this->construct_mode) {
		mode_str += "Construct mode";
	} else {
		mode_str += "Actions mode";
		if (this->use_set_ability) {
			mode_str += " (" + std::to_string(this->ability) + ")";
		}
	}
	mode_str += " (player " + std::to_string(engine.current_player) + ")";
	engine.render_text({x, y}, 20, mode_str.c_str());

	if (this->building_placement) {
		auto building_type = this->datamanager.get_type_index(this->editor_current_building);
		auto txt = building_type->default_texture();
		auto size = building_type->foundation_size;
		tile_range center = building_center(mousepos_tile.to_phys2().to_phys3(), size);
		txt->draw(center.draw.to_camgame(), 0, engine.current_player);
	}

	return true;
}

bool GameMain::on_drawhud() {
	Engine &e = Engine::get();

	if (this->construct_mode) {

		// draw the currently selected editor texture tile
		this->terrain->texture(this->editor_current_terrain)->draw(coord::window{63, 84}.to_camhud(), ALPHAMASKED);

		if (this->datamanager.producer_count() > 0) {
			// and the current active building
			coord::window bpreview_pos;
			bpreview_pos.x = e.engine_coord_data->window_size.x - 200;
			bpreview_pos.y = 200;

			auto txt = this->datamanager.get_type_index(this->editor_current_building)->default_texture();
			txt->sample(bpreview_pos.to_camhud(), engine->current_player);
		}
	}

	// draw the mighty GUI over everything
	gui::Drawer drawer{e.engine_coord_data->window_size.x, e.engine_coord_data->window_size.y};
	gui_top_level->resize(e.engine_coord_data->window_size.x, e.engine_coord_data->window_size.y); // FIXME only resize when size actually changes
	gui_top_level->update(0.0);
	gui_top_level->draw(drawer);

	return true;
}

void GameMain::draw_debug_grid() {
	Engine &e = Engine::get();

	coord::camgame camera = coord::tile{0, 0}.to_tile3().to_phys3().to_camgame();

	int cam_offset_x = util::mod(camera.x, e.engine_coord_data->tile_halfsize.x * 2);
	int cam_offset_y = util::mod(camera.y, e.engine_coord_data->tile_halfsize.y * 2);

	int line_half_width = e.engine_coord_data->window_size.x / 2;
	int line_half_height = e.engine_coord_data->window_size.y / 2;

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
	int k = line_half_width / (e.engine_coord_data->tile_halfsize.x);

	int tilesize_x = e.engine_coord_data->tile_halfsize.x * 2;
	int common_x   = cam_offset_x + e.engine_coord_data->tile_halfsize.x;
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

Command GameMain::get_action(const coord::phys3 &pos) const {
	auto obj = this->terrain->obj_at_point(pos);
	if (obj) {
		Command c(this->players[engine->current_player - 1], &obj->unit, pos);
		if (this->use_set_ability) {
			c.set_ability(ability);
		}
		return c;
	}
	else {
		Command c(this->players[engine->current_player - 1], pos);
		if (this->use_set_ability) {
			c.set_ability(ability);
		}
		return c;
	}

}

} //namespace openage
