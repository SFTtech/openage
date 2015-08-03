// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "util/strings.h"
#include "game_control.h"
#include "game_spec.h"

namespace openage {

CreateMode::CreateMode()
	:
	selected{0},
	setting_value{false} {

	// action bindings
	this->bind(input::action_t::START_GAME, [this](const input::action_arg_t &) {
		Engine &engine = Engine::get();
		options::OptionNode *node = engine.get_child("Generator");
		if (!node) {
			return;
		}

		std::vector<std::string> list = node->list_variables();
		std::vector<std::string> flist = node->list_functions();
		unsigned int size = list.size() + flist.size();
		unsigned int selected = util::mod<int>(this->selected, size);

		if (this->setting_value) {

			// modifying a value
			std::vector<std::string> list = node->list_variables();
			std::string selected_name = list[selected];
			auto &var = node->get_variable_rw(selected_name);

			// try change the value
			// deal with number parsing exceptions
			try {
				if (var.type == options::option_type::list_type) {
					auto list = var.value<options::option_list>();
					list.push_back(this->new_value);
					var = list;
					log::log(MSG(dbg) << selected_name << " appended with " << this->new_value);
				}
				else {
					var = options::parse(var.type, this->new_value);
					log::log(MSG(dbg) << selected_name << " set to " << this->new_value);
				}
			}
			catch (const std::invalid_argument &e) {
				log::log(MSG(dbg) << "exception setting " << selected_name << " to " << this->new_value);
			}
			catch (const std::out_of_range &e) {
				log::log(MSG(dbg) << "exception setting " << selected_name << " to " << this->new_value);
			}

			this->setting_value = false;
			this->utf8_mode = false;
			this->new_value = "";
		}
		else {
			if (selected >= list.size()) {
				node->do_action(flist[selected - list.size()]);
			}
			else {
				this->setting_value = true;
				this->utf8_mode = true;
				this->new_value = "";
			}
		}

	});
	this->bind(input::action_t::UP_ARROW, [this](const input::action_arg_t &) {
		if (!this->setting_value) {
			this->selected -= 1;
		}
	});
	this->bind(input::action_t::DOWN_ARROW, [this](const input::action_arg_t &) {
		if (!this->setting_value) {
			this->selected += 1;
		}
	});
	this->bind(input::event_class::UTF8, [this](const input::action_arg_t &arg) {
		if (this->setting_value) {
			this->new_value += arg.e.as_utf8();
			return true;
		}
		return false;
	});
}

bool CreateMode::available() const {
	return true;
}

void CreateMode::on_enter() {}

void CreateMode::render() {
	Engine &engine = Engine::get();

	// initial draw positions
	int x = 75;
	int y = engine.engine_coord_data->window_size.y - 65;

	options::OptionNode *node = engine.get_child("Generator");
	if (node) {
		unsigned int i = 0;


		// list everything
		std::vector<std::string> list = node->list_variables();
		std::vector<std::string> flist = node->list_functions();
		unsigned int size = list.size() + flist.size();
		unsigned int selected = util::mod<int>(this->selected, size);

		for (auto &s : list) {
			engine.render_text({x, y}, 20, "%s", s.c_str());
			if (i == selected) {
				engine.render_text({x - 35, y}, 20, ">>");
			}
			auto var = node->get_variable(s);
			engine.render_text({x + 320, y}, 20, "%s", var.str_value().c_str());
			y -= 24;
			i++;
		}
		for (auto &s : flist) {
			engine.render_text({x, y}, 20, "%s", (s + "()").c_str());
			if (i == selected) {
				engine.render_text({x - 35, y}, 20, ">>");
			}
			y -= 24;
			i++;
		}

		if (this->setting_value) {
			y -= 36;
			engine.render_text({x + 45, y}, 20, "set value:");
			engine.render_text({x + 320, y}, 20, "%s", this->new_value.c_str());
			y -= 24;
		}
	}
	else {
		engine.render_text({0, 100}, 12, "No generator");
	}


	// TODO show all bindings
	engine.render_text({0, 160}, 12, "Return -> use selection");
	engine.render_text({0, 140}, 12, "M      -> toggle modes");
}

ActionMode::ActionMode()
	:
	use_set_ability{false},
	type_focus{nullptr},
	rng{0} {

	this->bind(input::action_t::TRAIN_OBJECT, [this](const input::action_arg_t &) {

		// attempt to train editor selected object
		Engine &engine = Engine::get();
		GameSpec *spec = engine.get_game()->get_spec();

		// randomly select between male and female villagers
		auto type = spec->get_type(this->rng.probability(0.5)? 83 : 293);
		Command cmd(*engine.player_focus(), type);
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
	this->bind(input::action_t::SET_ABILITY_GARRISON, [this](const input::action_arg_t &) {
		this->use_set_ability = true;
		this->ability = ability_type::garrison;
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
		if (arg.e.cc == input::ClassCode(input::event_class::MOUSE_MOTION, 0) &&
			engine.get_input_manager().is_down(input::event_class::MOUSE_BUTTON, 1)) {
			this->selection.drag_update(mousepos_camgame);
			return true;
		}
		else if (arg.e.cc == input::ClassCode(input::event_class::MOUSE_BUTTON, 1)) {
			if (this->type_focus) {
				this->place_selection(this->mousepos_phys3);

				// shift click can place many buildings
				if (!engine.get_input_manager().is_mod_down(input::modifier::SHIFT)) {
					this->type_focus = nullptr;
					return false;
				}
			}
			else {
				log::log(MSG(dbg) << "select");
				Terrain *terrain = engine.get_game()->terrain.get();
				this->selection.drag_update(mousepos_camgame);
				this->selection.drag_release(terrain, engine.get_input_manager().is_mod_down(input::modifier::CTRL));
			}
			return true;
		}
		else if (arg.e.cc == input::ClassCode(input::event_class::MOUSE_BUTTON, 3)) {
			this->on_single_click(0, arg.mouse);
			return true;
		}
		return false;
	});

}

bool ActionMode::available() const {
	Engine &engine = Engine::get();
	if (engine.get_game()) {
		return true;
	}
	else {
		log::log(MSG(warn) << "Cannot enter action mode without a game");
		return false;
	}
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

bool ActionMode::place_selection(coord::phys3 point) {
	if (this->type_focus) {

		// confirm building placement with left click
		// first create foundation using the producer
		Engine &engine = Engine::get();
		UnitContainer *container = &engine.get_game()->placed_units;
		UnitReference new_building = container->new_unit(*this->type_focus, *engine.player_focus(), point);

		// task all selected villagers to build
		// TODO: editor placed objects are completed already
		if (new_building.is_valid()) {
			Command cmd(*engine.player_focus(), new_building.get());
			cmd.set_ability(ability_type::build);
			this->selection.all_invoke(cmd);
			return true;
		}
	}
	return false;
}

void ActionMode::render() {
	Engine &engine = Engine::get();
	if (engine.get_game()) {
		Player *player = engine.player_focus();
		// draw actions mode indicator
		int x = 5;
		int y = engine.engine_coord_data->window_size.y - 25;

		std::string status;
		status += "Food: " + std::to_string(static_cast<int>(player->amount(game_resource::food)));
		status += " | Wood: " + std::to_string(static_cast<int>(player->amount(game_resource::wood)));
		status += " | Gold: " + std::to_string(static_cast<int>(player->amount(game_resource::gold)));
		status += " | Stone: " + std::to_string(static_cast<int>(player->amount(game_resource::stone)));
		status += " | Actions mode";
		status += " ([" + std::to_string(player->color) + "] " + player->name + ")";
		if (this->use_set_ability) {
			status += " (" + std::to_string(this->ability) + ")";
		}
		engine.render_text({x, y}, 20, "%s", status.c_str());


		// when a building is being placed
		if (this->type_focus) {
			auto txt = this->type_focus->default_texture();
			auto size = this->type_focus->foundation_size;
			tile_range center = building_center(this->mousepos_phys3, size);
			txt->sample(center.draw.to_camgame().to_window().to_camhud(), player->color);
		}
	}
	else {
		engine.render_text({0, 140}, 12, "Action Mode requires a game");
	}

	this->selection.on_drawhud();
}

bool ActionMode::on_mouse_wheel(int, coord::window) {
	return false;
}

bool ActionMode::on_single_click(int, coord::window point) {
	if (this->type_focus) {

		// right click can cancel building placement
		this->type_focus = nullptr;
		return false;
	}

	auto mousepos_camgame = point.to_camgame();
	auto mousepos_phys3 = mousepos_camgame.to_phys3();

	auto cmd = this->get_action(mousepos_phys3);
	this->selection.all_invoke(cmd);
	this->use_set_ability = false;

	return false;
}

EditorMode::EditorMode()
	:
	editor_current_terrain{0},
	editor_current_type{0},
	editor_category{0},
	selected_type{nullptr},
	paint_terrain{true} {

	// bind required hotkeys
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
		if (arg.e.cc == input::ClassCode(input::event_class::MOUSE_BUTTON, 1) ||
			engine.get_input_manager().is_down(input::event_class::MOUSE_BUTTON, 1)) {
			this->on_single_click(0, arg.mouse);
			return true;
		}
		else if (arg.e.cc == input::ClassCode(input::event_class::MOUSE_BUTTON, 3) ||
			engine.get_input_manager().is_down(input::event_class::MOUSE_BUTTON, 3)) {
			this->on_single_click(1, arg.mouse);
			return true;
		}
		return false;
	});
}

bool EditorMode::available() const {
	Engine &engine = Engine::get();
	if (engine.get_game()) {
		return true;
	}
	else {
		log::log(MSG(warn) << "Cannot enter editor mode without a game");
		return false;
	}
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
			engine.render_text(text_pos, 20, "%s", ("Terrain " + std::to_string(this->editor_current_terrain)).c_str());
		}
		else if (this->selected_type) {
			// and the current active building
			auto txt = this->selected_type->default_texture();
			coord::window bpreview_pos {163, 154};
			txt->sample(bpreview_pos.to_camhud(), engine.player_focus()->color);

			std::string selected_str = this->category + " - " + this->selected_type->name();
			engine.render_text(text_pos, 20, "%s", selected_str.c_str());
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
	engine{engine},
	active_mode_index{0} {

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
	int next_mode = (this->active_mode_index + 1) % this->modes.size();
	if (this->modes[next_mode]->available()) {
		engine->get_input_manager().remove_context(this->active_mode);

		// set the new active mode
		this->active_mode_index = next_mode;
		this->active_mode = this->modes[next_mode].get();
		this->active_mode->on_enter();

		// update the context
		engine->get_input_manager().register_context(this->active_mode);
	}
}


bool GameControl::on_drawhud() {

	// render the active mode
	this->active_mode->render();
	return true;
}


} //namespace openage
