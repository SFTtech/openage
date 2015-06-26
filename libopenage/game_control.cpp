// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "util/strings.h"
#include "game_control.h"
#include "game_save.h"
#include "game_spec.h"

namespace openage {

CreateMode::CreateMode(Engine &engine)
	:
	assetmanager{engine.get_data_dir()} {

	// some default settings
	this->settings.number_of_players = 8;
	this->settings.spec = std::make_shared<GameSpec>(&this->assetmanager);
}

void CreateMode::render() {

	// TODO: move these somewhere else
	this->assetmanager.check_updates();
	this->settings.spec->check_updates();

	Engine &engine = Engine::get();
	if (!this->settings.spec->load_complete()) {
		// Show that gamedata is still loading
		engine.render_text({0, 100}, 20, "Loading gamedata...");
	}
}

bool CreateMode::on_mouse_wheel(int direction, coord::window point) {
	return false;
}

bool CreateMode::on_single_click(int button, coord::window point) {
	Engine &engine = Engine::get();
	engine.start_game(this->settings);
	return true;
}

bool CreateMode::on_drag_start(int button, coord::window point) {
	return false;
}

bool CreateMode::on_drag_end(int button, coord::window point) {
	return false;
}

ActionMode::ActionMode()
	:
	use_set_ability{false},
	type_focus{nullptr} {

	// this->engine->register_drawhud_action(&this->selection);

	this->bind(keybinds::action_t::TRAIN_OBJECT, [this]() {
		// attempt to train editor selected object
		if (this->type_focus) {
			Engine &engine = Engine::get();
			auto type = this->type_focus;
			Command cmd(*engine.player_focus(), type);
			this->selection.all_invoke(cmd);
		}
	});
	this->bind(keybinds::action_t::ENABLE_BUILDING_PLACEMENT, [this]() {
		// this->building_placement = true;
	});
	this->bind(keybinds::action_t::DISABLE_SET_ABILITY, [this]() {
		this->use_set_ability = false;
	});
	this->bind(keybinds::action_t::SET_ABILITY_MOVE, [this]() {
		this->use_set_ability = true;
		this->ability = ability_type::move;
	});
	this->bind(keybinds::action_t::SET_ABILITY_GATHER, [this]() {
		this->use_set_ability = true;
		this->ability = ability_type::gather;
	});
	this->bind(keybinds::action_t::SPAWN_VILLAGER, [this]() {
		Engine &engine = Engine::get();
		GameSpec *spec = engine.get_game()->get_spec();
		if (spec->producer_count() > 0) {
			UnitType &type = *spec->get_type(590);

			// TODO tile position
			//engine.get_game()->placed_units.new_unit(type, *engine.player_focus(), mousepos_tile.to_phys2().to_phys3());
		}
	});
	this->bind(keybinds::action_t::KILL_UNIT, [this]() {
		selection.kill_unit();
	});

	// Villager build commands
	// TODO place this into separate building menus instead of global hotkeys
	auto bind_building_key = [this](keybinds::action_t action, int building, int military_building) {
		this->bind(action, [this, building, military_building]() {
			if (this->selection.contains_builders()) {
				//this->building_placement = true;
				Engine &engine = Engine::get();
				if (engine.get_keybind_manager().is_keymod_down(KMOD_LCTRL)) {
					//this->type_focus = military_building;
				} else {
					//this->type_focus = building;
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
		this->bind(action, [this, player]() {
			Engine &engine = Engine::get();
			engine.current_player = player;
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


	// engine->get_keybind_manager().register_context(&this->keybind_context);
}

Command ActionMode::get_action(const coord::phys3 &pos) const {
	Engine &engine = Engine::get();
	GameMain *game = engine.get_game();
	auto obj = game->terrain->obj_at_point(pos);
	if (obj) {
		Command c(*engine.player_focus(), &obj->unit, pos);
		if (this->use_set_ability) {
			c.set_ability(ability);
		}
		return c;
	}
	else {
		Command c(*engine.player_focus(), pos);
		if (this->use_set_ability) {
			c.set_ability(ability);
		}
		return c;
	}
}

void ActionMode::render() {
	Engine &engine = Engine::get();

	// draw construction or actions mode indicator
	int x = 400 - (engine.engine_coord_data->window_size.x / 2);
	int y = 35 - (engine.engine_coord_data->window_size.y / 2);

	std::string mode_str;
	mode_str += "Actions mode";
	if (this->use_set_ability) {
		mode_str += " (" + std::to_string(this->ability) + ")";
	}

	mode_str += " (player " + std::to_string(engine.current_player) + ")";
	engine.render_text({x, y}, 20, mode_str.c_str());

	// when a building is being placed
	if (this->type_focus) {
		auto txt = this->type_focus->default_texture();
		auto size = this->type_focus->foundation_size;
		tile_range center = building_center(mousepos_tile.to_phys2().to_phys3(), size);
		txt->draw(center.draw.to_camgame(), 0, engine.current_player);
	}
}

bool ActionMode::on_mouse_wheel(int direction, coord::window point) {
	return false;
}

bool ActionMode::on_single_click(int button, coord::window point) {
	if (this->type_focus) {

		// confirm building placement with left click
		// first create foundation using the producer
		//UnitContainer *container = &engine.get_game()->placed_units;
		//UnitReference new_building = container->new_unit(*type_focus, *engine.player_focus(), mousepos_phys3);

		// task all selected villagers to build
		// TODO: editor placed objects are completed already
		// if (new_building.is_valid()) {
		// 	Command cmd(*engine.player_focus(), new_building.get());
		// 	cmd.set_ability(ability_type::build);
		// 	this->selection.all_invoke(cmd);
		// }

		// right click can cancel building placement
		this->type_focus = nullptr;
	}


	auto mousepos_camgame = point.to_camgame();
	auto mousepos_phys3 = mousepos_camgame.to_phys3();

	auto cmd = this->get_action(mousepos_phys3);
	selection.all_invoke(cmd);

	return false;
}
bool ActionMode::on_drag_start(int button, coord::window point) {

	//selection.drag_begin(mousepos_camgame);
	//dragging_active = true;

	return false;
}

bool ActionMode::on_drag_end(int button, coord::window point) {

	//selection.drag_update(mousepos_camgame);

	//selection.drag_release(terrain, this->engine->get_keybind_manager().is_keymod_down(KMOD_LCTRL));
	//dragging_active = false;

	return false;
}

EditorMode::EditorMode()
	:
	editor_current_terrain{0},
	editor_current_building{0},
	paint_terrain{true},
	building_placement{false} {
}

void EditorMode::render() {

	// Get the current game
	Engine &engine = Engine::get();
	GameMain *game = engine.get_game();

	if (game) {
		coord::window bpreview_pos{63, 84};
		coord::window text_pos{12, engine.engine_coord_data->window_size.y - 24};

		// draw the currently selected editor item
		if (this->paint_terrain && game->terrain) {
			game->terrain->texture(this->editor_current_terrain)->draw(bpreview_pos.to_camhud(), ALPHAMASKED);
			engine.render_text(text_pos, 20, ("Terrain " + std::to_string(this->editor_current_terrain)).c_str());
		}
		else if (game->get_spec()->producer_count() > 0) {
			// and the current active building
			auto txt = game->get_spec()->get_type_index(this->editor_current_building)->default_texture();
			txt->sample(bpreview_pos.to_camhud(), engine.current_player);
			engine.render_text(text_pos, 20, "Type");
		}
	}
}

bool EditorMode::on_mouse_wheel(int direction, coord::window point) {
	Engine &engine = Engine::get();
	GameSpec *spec = engine.get_game()->get_spec();

	// modify selected item
	if (engine.get_keybind_manager().is_keymod_down(KMOD_LCTRL) && spec->producer_count() > 0) {
		editor_current_building = util::mod<ssize_t>(editor_current_building + direction, spec->producer_count());
	} else {
		editor_current_terrain = util::mod<ssize_t>(editor_current_terrain + direction, spec->get_terrain_meta()->terrain_id_count);
	}
	return true;
}

bool EditorMode::on_single_click(int button, coord::window point) {
	Engine &engine = Engine::get();
	Terrain *terrain = engine.get_game()->terrain.get();

	auto mousepos_camgame = point.to_camgame();
	auto mousepos_phys3 = mousepos_camgame.to_phys3();
	auto mousepos_tile = mousepos_phys3.to_tile3().to_tile();

	log::log(MSG(dbg) <<
	    "LMB [window]:   "
	    " x " << std::setw(9) << point.x <<
	    " y " << std::setw(9) << point.y);

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
	if (this->paint_terrain) {
		chunk->get_data(mousepos_tile)->terrain_id = editor_current_terrain;
	}
	else {

		// delete any existing unit on the tile
		if (!chunk->get_data(mousepos_tile)->obj.empty()) {

			// get first object currently standing at the clicked position
			TerrainObject *obj = chunk->get_data(mousepos_tile)->obj[0];
			log::log(MSG(dbg) << "delete unit with unit id " << obj->unit.id);
			obj->unit.delete_unit();

		} else if (engine.get_game()->get_spec()->producer_count() > 0) {

			// try creating a unit
			log::log(MSG(dbg) << "create unit with producer id " << this->editor_current_building);
			UnitContainer *container = &engine.get_game()->placed_units;
			UnitType &type = *engine.get_game()->get_spec()->get_type_index(this->editor_current_building);
			container->new_unit(type, *engine.player_focus(), mousepos_phys3);
		}

	}
	return false;
}

bool EditorMode::on_drag_start(int button, coord::window point) {
	return false;
}

bool EditorMode::on_drag_end(int button, coord::window point) {
	return false;
}

GameControl::GameControl(openage::Engine *engine)
	:
	engine{engine},
	clicking_active{true},
	scrolling_active{false},
	dragging_active{false} {

	// add handlers
	engine->register_drawhud_action(this);
	engine->register_input_action(this);

	// modes list
	this->modes.push_back(std::make_unique<CreateMode>(*engine));
	this->modes.push_back(std::make_unique<EditorMode>());
	this->active_mode = modes.front().get();

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
	global_keybind_context.bind(keybinds::action_t::TOGGLE_CONSTRUCT_MODE, [this]() {
		this->toggle_mode();
	});
	global_keybind_context.bind(keybinds::action_t::QUICK_SAVE, [this]() {
		gameio::save(this->engine->get_game(), "default_save.txt");
	});
	global_keybind_context.bind(keybinds::action_t::QUICK_LOAD, [this]() {
		gameio::load(this->engine->get_game(), "default_save.txt");
	});
	global_keybind_context.bind(keybinds::action_t::TOGGLE_PROFILER, [this]() {
		if (this->external_profiler.currently_profiling) {
			this->external_profiler.stop();
			this->external_profiler.show_results();
		} else {
			this->external_profiler.start();
		}
	});
}

void GameControl::move_camera() {
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

void GameControl::toggle_mode() {
	this->active_mode_index = (this->active_mode_index + 1) % this->modes.size();
	this->active_mode = this->modes[this->active_mode_index].get();
}

bool GameControl::on_input(SDL_Event *e) {
	Engine &engine = Engine::get();

	switch (e->type) {

	case SDL_QUIT:
		engine.stop();
		break;

	case SDL_MOUSEBUTTONDOWN: {
		switch (e->button.button) {
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

		// pass to the active mode
		this->active_mode->on_single_click(e->button.button, mousepos_window);

		switch (e->button.button) {

		case SDL_BUTTON_LEFT:
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

		// scroll, if middle mouse is being pressed
		//  SDL_GetRelativeMouseMode() queries sdl for that.
		if (this->scrolling_active) {
			engine.move_phys_camera(e->motion.xrel, e->motion.yrel);
		}
		break;
	}

	case SDL_MOUSEWHEEL: {
		coord::window mousepos_window {(coord::pixel_t) e->button.x, (coord::pixel_t) e->button.y};
		this->active_mode->on_mouse_wheel(e->wheel.y, mousepos_window);
		break;
	}

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

bool GameControl::on_drawhud() {

	// move this somewhere else
	this->move_camera();

	// render the active mode
	this->active_mode->render();
	return true;
}


} //namespace openage