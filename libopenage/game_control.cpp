// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "util/strings.h"
#include "game_control.h"
#include "game_save.h"
#include "game_spec.h"

namespace openage {

CreateMode::CreateMode() {

	// action bindings
	this->bind(input::action_t::START_GAME, [this](const input::action_arg_t &) {
		log::log(MSG(dbg) << "start game");
		Engine &engine = Engine::get();
		options::OptionNode *n = engine.get_child("Generator");
		if (n) {
			n->do_action("start");
		}
		// engine.start_game(this->settings);
	});
	this->bind(input::action_t::STOP_GAME, [this](const input::action_arg_t &) {
		log::log(MSG(dbg) << "stop game");
		Engine &engine = Engine::get();
		engine.end_game();
	});

	// TODO add load and save here
	this->bind(input::action_t::QUICK_SAVE, [this](const input::action_arg_t &) {
		if (true) {
			log::log(MSG(warn) << "game required for saving");
			return;
		}
		//gameio::save(this->get_game(), "default_save.txt");
	});
	this->bind(input::action_t::QUICK_LOAD, [this](const input::action_arg_t &) {
		if (true) {
			log::log(MSG(warn) << "close existing game before loading");
			return;
		}
		//gameio::load(this->get_game(), "default_save.txt");
	});
}

void CreateMode::on_enter() {}

void CreateMode::render() {
	Engine &engine = Engine::get();

	if (engine.get_child("Generator")) {

		// Show that gamedata is still loading
		engine.render_text({0, 100}, 12, "Loading gamedata...");
	}
	else {
		engine.render_text({0, 100}, 12, "No generator");
	}

	engine.render_text({0, 160}, 12, "Return -> start a new game");
	engine.render_text({0, 140}, 12, "M      -> toggle modes");
}

ActionMode::ActionMode()
	:
	use_set_ability{false},
	type_focus{nullptr} {

	this->bind(input::action_t::TRAIN_OBJECT, [this](const input::action_arg_t &) {

		// attempt to train editor selected object
		Engine &engine = Engine::get();
		GameSpec *spec = engine.get_game()->get_spec();
		Command cmd(*engine.player_focus(), spec->get_type(590));
		this->selection.all_invoke(cmd);
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
			engine.get_game()->placed_units.new_unit(type, *engine.player_focus(), this->mousepos_phys3);
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
				Engine &engine = Engine::get();
				GameSpec *spec = engine.get_game()->get_spec();
				if (engine.get_input_manager().is_mod_down(input::modifier::CTRL)) {
					this->type_focus = spec->get_type_index(military_building);
				} else {
					this->type_focus = spec->get_type_index(building);
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

		auto mousepos_camgame = arg.mouse.to_camgame();
		this->mousepos_phys3 = mousepos_camgame.to_phys3();
		this->mousepos_tile = this->mousepos_phys3.to_tile3().to_tile();

		// drag selection box
		if (arg.e.cc == input::class_code_t(input::event_class::MOUSE_MOTION, 0) &&
			engine.get_input_manager().is_down(input::event_class::MOUSE_BUTTON, 1)) {
			this->selection.drag_update(mousepos_camgame);
			return true;
		}
		else if (arg.e.cc == input::class_code_t(input::event_class::MOUSE_BUTTON, 1)) {
			if (this->type_focus) {
				this->on_single_click(0, arg.mouse);
			}
			else {
				log::log(MSG(dbg) << "select");
				Terrain *terrain = engine.get_game()->terrain.get();
				this->selection.drag_update(mousepos_camgame);
				this->selection.drag_release(terrain, engine.get_input_manager().is_mod_down(input::modifier::CTRL));
			}
			return true;
		}
		else if (arg.e.cc == input::class_code_t(input::event_class::MOUSE_BUTTON, 3)) {
			this->on_single_click(0, arg.mouse);
			return true;
		}
		return false;
	});

}

void ActionMode::on_enter() {
	this->selection.clear();
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

	// draw actions mode indicator
	int x = 5;
	int y = engine.engine_coord_data->window_size.y - 25;

	std::string status;
	status += "F: 0 | W: 0 | G: 0 | S: 0 | Actions mode";
	if (this->use_set_ability) {
		status += " (" + std::to_string(this->ability) + ")";
	}
	status += " (player " + std::to_string(engine.current_player) + ")";
	engine.render_text({x, y}, 20, status.c_str());

	// when a building is being placed
	if (this->type_focus) {
		auto txt = this->type_focus->default_texture();
		auto size = this->type_focus->foundation_size;
		tile_range center = building_center(this->mousepos_phys3, size);
		txt->sample(center.draw.to_camgame().to_window().to_camhud(), engine.current_player);
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
		Engine &engine = Engine::get();
		UnitContainer *container = &engine.get_game()->placed_units;
		UnitReference new_building = container->new_unit(*this->type_focus, *engine.player_focus(), this->mousepos_phys3);

		// task all selected villagers to build
		// TODO: editor placed objects are completed already
		if (new_building.is_valid()) {
			Command cmd(*engine.player_focus(), new_building.get());
			cmd.set_ability(ability_type::build);
			this->selection.all_invoke(cmd);
		}

		// right click can cancel building placement
		this->type_focus = nullptr;
		return false;
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
	editor_current_type{0},
	editor_category{0},
	selected_type{nullptr},
	paint_terrain{true},
	building_placement{false} {

	this->bind(input::action_t::ENABLE_BUILDING_PLACEMENT, [this](const input::action_arg_t &) {
		log::log(MSG(dbg) << "change category");
		Engine &engine = Engine::get();
		GameSpec *spec = engine.get_game()->get_spec();
		std::vector<std::string> cat = spec->get_type_categories();
		if (this->paint_terrain) {

			// switch from terrain to first unit category
			this->paint_terrain = false;
			this->editor_category = 0;
		}
		else {
			this->editor_category += 1;

			// passed last category, switch back to terrain
			if (cat.size() <= this->editor_category) {
				this->paint_terrain = true;
				this->editor_category = 0;
			}
		}

		// update category string
		if (!cat.empty()) {
			this->category = cat[this->editor_category];
			std::vector<index_t> inds = spec->get_category(this->category);
			if (!inds.empty()) {
				this->editor_current_type = util::mod<ssize_t>(editor_current_type, inds.size());
				this->selected_type = spec->get_type(inds[this->editor_current_type]);
			}
		}
		return true;
	});

	this->bind(input::action_t::FORWARD, [this](const input::action_arg_t &arg) {
		this->on_mouse_wheel(1, arg.mouse);
	});

	this->bind(input::action_t::BACK, [this](const input::action_arg_t &arg) {
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
		coord::window text_pos{12, engine.engine_coord_data->window_size.y - 24};

		// draw the currently selected editor item
		if (this->paint_terrain && game->terrain) {
			coord::window bpreview_pos{63, 84};
			game->terrain->texture(this->editor_current_terrain)->draw(bpreview_pos.to_camhud(), ALPHAMASKED);
			engine.render_text(text_pos, 20, ("Terrain " + std::to_string(this->editor_current_terrain)).c_str());
		}
		else if (selected_type) {
			// and the current active building
			auto txt = selected_type->default_texture();
			coord::window bpreview_pos{163, 124};
			txt->sample(bpreview_pos.to_camhud(), engine.current_player);
			engine.render_text(text_pos, 20, this->category.c_str());
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
		std::vector<index_t> inds = spec->get_category(this->category);
		if (!inds.empty()) {
			this->editor_current_type = util::mod<ssize_t>(editor_current_type + direction, inds.size());
			this->selected_type = spec->get_type(inds[this->editor_current_type]);
		}
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

		} else if (!del && selected_type) {

			// tile is empty so try creating a unit
			log::log(MSG(dbg) << "create unit with producer id " << this->selected_type->id());
			UnitContainer *container = &engine.get_game()->placed_units;
			container->new_unit(*this->selected_type, *engine.player_focus(), mousepos_phys3);
		}

	}
	return false;
}

GameControl::GameControl(openage::Engine *engine)
	:
	engine{engine} {

	// add handlers
	engine->register_drawhud_action(this);

	// modes list
	this->modes.push_back(std::make_unique<CreateMode>());
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
