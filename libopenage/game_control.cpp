// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "util/strings.h"
#include "game_control.h"
#include "game_spec.h"

namespace openage {

CreateMode::CreateMode(Engine &engine)
	:
	assetmanager{engine.get_data_dir()} {

	// some default settings
	this->settings.number_of_players = 8;
	this->settings.spec = std::make_shared<GameSpec>(&this->assetmanager);

	this->bind(input::action_t::START_GAME, [this](const input::action_arg_t &) {
		log::log(MSG(dbg) << "start game");
		Engine &engine = Engine::get();
		engine.start_game(this->settings);
	});
}

void CreateMode::on_enter() {}

void CreateMode::render() {

	// TODO: move these somewhere else
	this->assetmanager.check_updates();
	this->settings.spec->check_updates();

	Engine &engine = Engine::get();
	if (!this->settings.spec->load_complete()) {
		// Show that gamedata is still loading
		engine.render_text({0, 100}, 12, "Loading gamedata...");
	}

	engine.render_text({0, 160}, 12, "Return -> start a new game");
	engine.render_text({0, 140}, 12, "M      -> toggle modes");
}

ActionMode::ActionMode()
	:
	use_set_ability{false},
	type_focus{nullptr} {

	// this->engine->register_drawhud_action(&this->selection);

	this->bind(input::action_t::TRAIN_OBJECT, [this](const input::action_arg_t &) {
		// attempt to train editor selected object
		if (this->type_focus) {
			Engine &engine = Engine::get();
			auto type = this->type_focus;
			Command cmd(*engine.player_focus(), type);
			this->selection.all_invoke(cmd);
		}
	});
	this->bind(input::action_t::ENABLE_BUILDING_PLACEMENT, [this](const input::action_arg_t &) {
		// this->building_placement = true;
	});
	this->bind(input::action_t::DISABLE_SET_ABILITY, [this](const input::action_arg_t &) {
		this->use_set_ability = false;
	});
	this->bind(input::action_t::SET_ABILITY_MOVE, [this](const input::action_arg_t &) {
		this->use_set_ability = true;
		this->ability = ability_type::move;
	});
	this->bind(input::action_t::SET_ABILITY_GATHER, [this](const input::action_arg_t &) {
		this->use_set_ability = true;
		this->ability = ability_type::gather;
	});
	this->bind(input::action_t::SPAWN_VILLAGER, [this](const input::action_arg_t &) {
		Engine &engine = Engine::get();
		GameSpec *spec = engine.get_game()->get_spec();
		if (spec->producer_count() > 0) {
			UnitType &type = *spec->get_type(590);

			// TODO tile position
			//engine.get_game()->placed_units.new_unit(type, *engine.player_focus(), mousepos_tile.to_phys2().to_phys3());
		}
	});
	this->bind(input::action_t::KILL_UNIT, [this](const input::action_arg_t &) {
		selection.kill_unit();
	});

	// Villager build commands
	// TODO place this into separate building menus instead of global hotkeys
	auto bind_building_key = [this](input::action_t action, int building, int military_building) {
		this->bind(action, [this, building, military_building](const input::action_arg_t &) {
			if (this->selection.contains_builders()) {
				//this->building_placement = true;
				Engine &engine = Engine::get();
				if (engine.get_input_manager().is_mod_down(KMOD_LCTRL)) {
					//this->type_focus = military_building;
				} else {
					//this->type_focus = building;
				}
			}
		});
	};
	bind_building_key(input::action_t::BUILDING_1, 598, 609); // House, barracks
	bind_building_key(input::action_t::BUILDING_2, 574, 558); // Mill, archery range
	bind_building_key(input::action_t::BUILDING_3, 616, 581); // Mining camp, stable
	bind_building_key(input::action_t::BUILDING_4, 611, 580); // Lumber camp, siege workshop
	bind_building_key(input::action_t::BUILDING_TOWN_CENTER, 568, 568); // Town center


	this->bind(input::event_class::MOUSE, [this](const input::action_arg_t &arg) {
		Engine &engine = Engine::get();
		if (arg.e.cc == input::class_code_t(input::event_class::MOUSE_MOTION, 0) &&
			engine.get_input_manager().is_down(input::event_class::MOUSE_BUTTON, 1)) {
			coord::camgame mousepos_camgame = arg.mouse.to_camgame();
			this->selection.drag_update(mousepos_camgame);
			return true;
		}
		else if (arg.e.cc == input::class_code_t(input::event_class::MOUSE_BUTTON, 1)) {
			coord::camgame mousepos_camgame = arg.mouse.to_camgame();
			log::log(MSG(dbg) << "select");
			Terrain *terrain = engine.get_game()->terrain.get();
			this->selection.drag_update(mousepos_camgame);
			this->selection.drag_release(terrain, engine.get_input_manager().is_mod_down(KMOD_LCTRL));
			return true;
		}
		else if (arg.e.cc == input::class_code_t(input::event_class::MOUSE_BUTTON, 3)) {
			this->on_single_click(0, arg.mouse);
		}
		return false;
	});

}

void ActionMode::on_enter() {}

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
	int x = 200; // - (engine.engine_coord_data->window_size.x / 2);
	int y = 135; // - (engine.engine_coord_data->window_size.y / 2);

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

	this->selection.on_drawhud();
}

bool ActionMode::on_mouse_wheel(int, coord::window) {
	return false;
}

bool ActionMode::on_single_click(int, coord::window point) {
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
	this->selection.all_invoke(cmd);

	return false;
}

EditorMode::EditorMode()
	:
	editor_current_terrain{0},
	editor_current_building{0},
	paint_terrain{true},
	building_placement{false} {


	this->bind(input::action_t::TOGGLE_MENU, [this](const input::action_arg_t &) {
		log::log(MSG(dbg) << "toggle");
		this->paint_terrain = !this->paint_terrain;
		return true;
	});

	this->bind(input::action_t::ENABLE_BUILDING_PLACEMENT, [this](const input::action_arg_t &) {
		log::log(MSG(dbg) << "toggle");
		this->paint_terrain = !this->paint_terrain;
	});

	this->bind(input::action_t::FORWARD, [this](const input::action_arg_t &arg) {
		log::log(MSG(dbg) << "wheel");
		this->on_mouse_wheel(1, arg.mouse);
	});

	this->bind(input::action_t::BACK, [this](const input::action_arg_t &arg) {
		log::log(MSG(dbg) << "wheel");
		this->on_mouse_wheel(-1, arg.mouse);
	});

	this->bind(input::event_class::MOUSE, [this](const input::action_arg_t &arg) {
		Engine &engine = Engine::get();
		if (arg.e.cc == input::class_code_t(input::event_class::MOUSE_BUTTON, 1) ||
			engine.get_input_manager().is_down(input::event_class::MOUSE_BUTTON, 1)) {
			this->on_single_click(0, arg.mouse);
			return true;
		}
		else if (arg.e.cc == input::class_code_t(input::event_class::MOUSE_BUTTON, 3) ||
			engine.get_input_manager().is_down(input::event_class::MOUSE_BUTTON, 3)) {
			this->on_single_click(1, arg.mouse);
			return true;
		}
		return false;
	});
}

void EditorMode::on_enter() {}

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

		engine.render_text({0, 180}, 12, "Wheel  -> change item");
		engine.render_text({0, 160}, 12, "Y      -> change menu");
		engine.render_text({0, 140}, 12, "M      -> toggle modes");
	}
	else {
		engine.render_text({0, 140}, 12, "Editor Mode requires a game");
	}
}

bool EditorMode::on_mouse_wheel(int direction, coord::window) {
	Engine &engine = Engine::get();
	GameSpec *spec = engine.get_game()->get_spec();

	// modify selected item
	if (this->paint_terrain) {
		editor_current_terrain = util::mod<ssize_t>(editor_current_terrain + direction, spec->get_terrain_meta()->terrain_id_count);
	} else if (spec->producer_count() > 0) {
		editor_current_building = util::mod<ssize_t>(editor_current_building + direction, spec->producer_count());
	}
	return true;
}

bool EditorMode::on_single_click(int del, coord::window point) {
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
			if (del) {

				// delete first object currently standing at the clicked position
				TerrainObject *obj = chunk->get_data(mousepos_tile)->obj[0];
				log::log(MSG(dbg) << "delete unit with unit id " << obj->unit.id);
				obj->unit.delete_unit();
			}

		} else if (!del && engine.get_game()->get_spec()->producer_count() > 0) {

			// try creating a unit
			log::log(MSG(dbg) << "create unit with producer id " << this->editor_current_building);
			UnitContainer *container = &engine.get_game()->placed_units;
			UnitType &type = *engine.get_game()->get_spec()->get_type_index(this->editor_current_building);
			container->new_unit(type, *engine.player_focus(), mousepos_phys3);
		}

	}
	return false;
}

bool EditorMode::on_drag_start(int, coord::window) {
	return false;
}

bool EditorMode::on_drag_end(int, coord::window) {
	return false;
}

GameControl::GameControl(openage::Engine *engine)
	:
	engine{engine} {

	// add handlers
	engine->register_drawhud_action(this);

	// modes list
	this->modes.push_back(std::make_unique<CreateMode>(*engine));
	this->modes.push_back(std::make_unique<EditorMode>());
	this->modes.push_back(std::make_unique<ActionMode>());

	// initial active mode
	this->active_mode = modes.front().get();
	engine->get_input_manager().register_context(this->active_mode);

	auto &global_input_context = engine->get_input_manager().get_global_context();
	global_input_context.bind(input::action_t::TOGGLE_CONSTRUCT_MODE, [this](const input::action_arg_t &) {
		this->toggle_mode();
	});

}

void GameControl::toggle_mode() {
	this->active_mode_index = (this->active_mode_index + 1) % this->modes.size();
	this->active_mode = this->modes[this->active_mode_index].get();
	this->active_mode->on_enter();

	// update the context
	engine->get_input_manager().remove_context();
	engine->get_input_manager().register_context(this->active_mode);
}


bool GameControl::on_drawhud() {

	// render the active mode
	this->active_mode->render();
	return true;
}


} //namespace openage
