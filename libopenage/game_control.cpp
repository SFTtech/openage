// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gamestate/game_spec.h"
#include "util/strings.h"
#include "game_control.h"

namespace openage {

OutputMode::OutputMode(qtsdl::GuiItemLink *gui_link)
	:
	game_control{},
	gui_link{gui_link} {
}

void OutputMode::announce() {
	emit this->gui_signals.announced(this->name(), this->active_binds());
}

void OutputMode::set_game_control(GameControl *game_control) {
	this->game_control = game_control;
}

CreateMode::CreateMode(qtsdl::GuiItemLink *gui_link)
	:
	OutputMode{gui_link} {
}

bool CreateMode::available() const {
	return true;
}

std::string CreateMode::name() const {
	return "Creation Mode";
}


void CreateMode::on_enter() {}

void CreateMode::on_exit() {}

void CreateMode::render() {}

ActionMode::ActionMode(qtsdl::GuiItemLink *gui_link)
	:
	OutputMode{gui_link},
	use_set_ability{false},
	type_focus{nullptr},
	rng{0} {

	auto &action = Engine::get().get_action_manager();
	this->bind(action.get("TRAIN_OBJECT"), [this](const input::action_arg_t &) {

		// attempt to train editor selected object

		// randomly select between male and female villagers
		auto player = this->game_control->get_current_player();
		auto type = player->get_type(this->rng.probability(0.5)? 83 : 293);
		Command cmd(*player, type);
		cmd.add_flag(command_flag::direct);
		this->selection.all_invoke(cmd);
	});
	this->bind(action.get("ENABLE_BUILDING_PLACEMENT"), [this](const input::action_arg_t &) {
		// this->building_placement = true;
	});
	this->bind(action.get("DISABLE_SET_ABILITY"), [this](const input::action_arg_t &) {
		this->use_set_ability = false;
	});
	this->bind(action.get("SET_ABILITY_MOVE"), [this](const input::action_arg_t &) {
		this->use_set_ability = true;
		this->ability = ability_type::move;
		emit this->gui_signals.ability_changed(std::to_string(this->ability));
	});
	this->bind(action.get("SET_ABILITY_GATHER"), [this](const input::action_arg_t &) {
		this->use_set_ability = true;
		this->ability = ability_type::gather;
		emit this->gui_signals.ability_changed(std::to_string(this->ability));
	});
	this->bind(action.get("SET_ABILITY_GARRISON"), [this](const input::action_arg_t &) {
		this->use_set_ability = true;
		this->ability = ability_type::garrison;
		emit this->gui_signals.ability_changed(std::to_string(this->ability));
	});
	this->bind(action.get("SPAWN_VILLAGER"), [this](const input::action_arg_t &) {
		Engine &engine = Engine::get();
		auto player = this->game_control->get_current_player();
		if (player->type_count() > 0) {
			UnitType &type = *player->get_type(590);

			// TODO tile position
			engine.get_game()->placed_units.new_unit(type, *player, this->mousepos_phys3);
		}
	});
	this->bind(action.get("KILL_UNIT"), [this](const input::action_arg_t &) {
		selection.kill_unit(*this->game_control->get_current_player());
	});

	// Villager build commands
	// TODO place this into separate building menus instead of global hotkeys
	auto bind_building_key = [this](input::action_t action, int building) {
		this->bind(action, [this, building](const input::action_arg_t &) {
			auto player = this->game_control->get_current_player();
			if (this->selection.contains_builders(*player)) {
				Engine &engine = Engine::get();
				auto player = this->game_control->get_current_player();
				this->type_focus = player->get_type(building);
				if (&engine.get_input_manager().get_top_context() != &this->building_context) {
					engine.get_input_manager().register_context(&this->building_context);
				}
			}
		});
	};
	bind_building_key(action.get("BUILDING_1"), 70); // House
	bind_building_key(action.get("BUILDING_2"), 68); // Mill
	bind_building_key(action.get("BUILDING_3"), 584); // Mining camp
	bind_building_key(action.get("BUILDING_4"), 562); // Lumber camp
	bind_building_key(action.get("BUILDING_5"), 12); // barracks
	bind_building_key(action.get("BUILDING_6"), 87); // archery range
	bind_building_key(action.get("BUILDING_7"), 101); // stable
	bind_building_key(action.get("BUILDING_8"), 49); // siege workshop
	bind_building_key(action.get("BUILDING_TOWN_CENTER"), 109); // Town center

	auto bind_build = [this](input::action_t action, const bool increase) {
		this->building_context.bind(action, [this, increase](const input::action_arg_t &arg) {
			auto mousepos_camgame = arg.mouse.to_camgame();
			this->mousepos_phys3 = mousepos_camgame.to_phys3();
			this->mousepos_tile = this->mousepos_phys3.to_tile3().to_tile();

			this->place_selection(this->mousepos_phys3);
			if (!increase) {
				this->type_focus = nullptr;
				Engine::get().get_input_manager().remove_context(&this->building_context);
			}
		});
	};

	bind_build(action.get("BUILD"), false);
	bind_build(action.get("KEEP_BUILDING"), true);

	auto bind_select = [this](input::action_t action, const bool increase) {
		this->bind(action, [this, increase](const input::action_arg_t &arg) {
			auto mousepos_camgame = arg.mouse.to_camgame();
			Engine &engine = Engine::get();
			Terrain *terrain = engine.get_game()->terrain.get();
			this->selection.drag_update(mousepos_camgame);
			this->selection.drag_release(*this->game_control->get_current_player(), terrain, increase);
		});
	};

	bind_select(action.get("SELECT"), false);
	bind_select(action.get("INCREASE_SELECTION"), true);

	this->bind(action.get("ORDER_SELECT"), [this](const input::action_arg_t &arg) {
		if (this->type_focus) {
			// right click can cancel building placement
			this->type_focus = nullptr;
			Engine::get().get_input_manager().remove_context(&this->building_context);
		}
		auto mousepos_camgame = arg.mouse.to_camgame();
		auto mousepos_phys3 = mousepos_camgame.to_phys3();

		auto cmd = this->get_action(mousepos_phys3);
		cmd.add_flag(command_flag::direct);
		this->selection.all_invoke(cmd);
		this->use_set_ability = false;
	});

	this->bind(input::event_class::MOUSE, [this](const input::action_arg_t &arg) {
		Engine &engine = Engine::get();

		auto mousepos_camgame = arg.mouse.to_camgame();
		this->mousepos_phys3 = mousepos_camgame.to_phys3();
		this->mousepos_tile = this->mousepos_phys3.to_tile3().to_tile();

		// drag selection box
		if (arg.e.cc == input::ClassCode(input::event_class::MOUSE_MOTION, 0) &&
			engine.get_input_manager().is_down(input::event_class::MOUSE_BUTTON, 1) && !this->type_focus) {
			this->selection.drag_update(mousepos_camgame);
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

void ActionMode::on_enter() {}

void ActionMode::on_exit() {
	this->selection.clear();
}

Command ActionMode::get_action(const coord::phys3 &pos) const {
	Engine &engine = Engine::get();
	GameMain *game = engine.get_game();
	auto obj = game->terrain->obj_at_point(pos);
	if (obj) {
		Command c(*this->game_control->get_current_player(), &obj->unit, pos);
		if (this->use_set_ability) {
			c.set_ability(ability);
		}
		return c;
	}
	else {
		Command c(*this->game_control->get_current_player(), pos);
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
		UnitReference new_building = container->new_unit(*this->type_focus, *this->game_control->get_current_player(), point);

		// task all selected villagers to build
		// TODO: editor placed objects are completed already
		if (new_building.is_valid()) {
			Command cmd(*this->game_control->get_current_player(), new_building.get());
			cmd.set_ability(ability_type::build);
			cmd.add_flag(command_flag::direct);
			this->selection.all_invoke(cmd);
			return true;
		}
	}
	return false;
}

void ActionMode::render() {
	Engine &engine = Engine::get();
	if (engine.get_game()) {
		Player *player = this->game_control->get_current_player();

		this->announce_resources();

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

std::string ActionMode::name() const {
	return "Action Mode";
}

void ActionMode::announce() {
	this->OutputMode::announce();

	this->announce_resources();
	emit this->gui_signals.ability_changed(this->use_set_ability ? std::to_string(this->ability) : "");
}

void ActionMode::announce_resources() {
	if (Player *player = this->game_control->get_current_player()) {
		for (auto i = static_cast<std::underlying_type<game_resource>::type>(game_resource::RESOURCE_TYPE_COUNT); i != 0; --i) {
			auto resource_type = static_cast<game_resource>(i - 1);
			emit this->gui_signals.resource_changed(resource_type, static_cast<int>(player->amount(resource_type)));
		}
	}
}

EditorModeSignals::EditorModeSignals(EditorMode *editor_mode)
	:
	editor_mode{editor_mode} {
}

void EditorModeSignals::on_current_player_name_changed() {
	this->editor_mode->announce_categories();
}

EditorMode::EditorMode(qtsdl::GuiItemLink *gui_link)
	:
	OutputMode{gui_link},
	editor_current_terrain{-1},
	current_type_id{-1},
	paint_terrain{},
	gui_signals{this} {

	auto &action = Engine::get().get_action_manager();

	// bind required hotkeys
	this->bind(action.get("ENABLE_BUILDING_PLACEMENT"), [this](const input::action_arg_t &) {
		log::log(MSG(dbg) << "change category");
		emit this->gui_signals.toggle();
	});

	this->bind(input::event_class::MOUSE, [this](const input::action_arg_t &arg) {
		Engine &engine = Engine::get();
		if (arg.e.cc == input::ClassCode(input::event_class::MOUSE_BUTTON, 1) ||
			engine.get_input_manager().is_down(input::event_class::MOUSE_BUTTON, 1)) {
			if (this->paint_terrain) {
				this->paint_terrain_at(arg.mouse);
			} else {
				this->paint_entity_at(arg.mouse, false);
			}
			return true;
		}
		else if (arg.e.cc == input::ClassCode(input::event_class::MOUSE_BUTTON, 3) ||
			engine.get_input_manager().is_down(input::event_class::MOUSE_BUTTON, 3)) {
				if (!this->paint_terrain) {
					this->paint_entity_at(arg.mouse, true);
				}
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

void EditorMode::on_exit() {}

void EditorMode::render() {}

std::string EditorMode::name() const {
	return "Editor mode";
}

void EditorMode::set_current_type_id(int current_type_id) {
	this->current_type_id = current_type_id;
}

void EditorMode::set_current_terrain_id(terrain_t current_terrain_id) {
	this->editor_current_terrain = current_terrain_id;
}

void EditorMode::set_paint_terrain(bool paint_terrain) {
	this->paint_terrain = paint_terrain;
}

void EditorMode::paint_terrain_at(const coord::window &point) {
	Engine &engine = Engine::get();
	Terrain *terrain = engine.get_game()->terrain.get();

	auto mousepos_camgame = point.to_camgame();
	auto mousepos_phys3 = mousepos_camgame.to_phys3();
	auto mousepos_tile = mousepos_phys3.to_tile3().to_tile();

	TerrainChunk *chunk = terrain->get_create_chunk(mousepos_tile);
	chunk->get_data(mousepos_tile)->terrain_id = editor_current_terrain;
}

void EditorMode::paint_entity_at(const coord::window &point, const bool del) {
	Engine &engine = Engine::get();
	Terrain *terrain = engine.get_game()->terrain.get();

	auto mousepos_camgame = point.to_camgame();
	auto mousepos_phys3 = mousepos_camgame.to_phys3();
	auto mousepos_tile = mousepos_phys3.to_tile3().to_tile();

	TerrainChunk *chunk = terrain->get_create_chunk(mousepos_tile);
	// TODO : better detection of presence of unit
	if (!chunk->get_data(mousepos_tile)->obj.empty()) {
		if (del) {
			// delete first object currently standing at the clicked position
			TerrainObject *obj = chunk->get_data(mousepos_tile)->obj[0];
			obj->remove();
		}
	} else if (!del && this->current_type_id != -1) {
		Player *player = this->game_control->get_current_player();
		UnitType *selected_type = player->get_type(this->current_type_id);

		// tile is empty so try creating a unit
		UnitContainer *container = &engine.get_game()->placed_units;
		container->new_unit(*selected_type, *this->game_control->get_current_player(), mousepos_phys3);
	}
}

void EditorMode::announce_categories() {
	if (this->game_control)
		if (auto player = this->game_control->get_current_player())
			emit this->gui_signals.categories_changed(player->civ->get_type_categories());

	emit this->gui_signals.categories_content_changed();
}

void EditorMode::announce_category_content(const std::string &category_name) {
	if (this->game_control)
		if (auto player = this->game_control->get_current_player()) {
			auto inds = player->civ->get_category(category_name);
			std::vector<std::tuple<index_t, uint16_t>> type_and_texture(inds.size());

			auto it = std::begin(type_and_texture);

			std::for_each(std::begin(inds), std::end(inds), [player, &it] (auto index) {
				*it++ = std::make_tuple(index, player->get_type(index)->default_texture()->id);
			});

			emit this->gui_signals.category_content_changed(category_name, type_and_texture);
		}
}

void EditorMode::announce() {
	OutputMode::announce();
	announce_categories();
}

void EditorMode::set_game_control(GameControl *game_control) {
	if (this->game_control != game_control) {
		if (this->game_control)
			QObject::disconnect(&this->game_control->gui_signals, &GameControlSignals::current_player_name_changed, &this->gui_signals, &EditorModeSignals::on_current_player_name_changed);

		OutputMode::set_game_control(game_control);

		if (this->game_control)
			QObject::connect(&this->game_control->gui_signals, &GameControlSignals::current_player_name_changed, &this->gui_signals, &EditorModeSignals::on_current_player_name_changed);

		announce_categories();
	} else {
		OutputMode::set_game_control(game_control);
	}
}

GameControlSignals::GameControlSignals(GameControl *game_control)
	:
	game_control{game_control} {
}

void GameControlSignals::on_game_running(bool running) {
	if (running)
		this->game_control->announce_current_player_name();
}

GameControl::GameControl(qtsdl::GuiItemLink *gui_link)
	:
	engine{},
	game{},
	active_mode{},
	active_mode_index{-1},
	current_player{1},
	gui_signals{this},
	gui_link{gui_link} {
}

void GameControl::set_engine(Engine *engine) {
	// TODO: decide to either go for a full Engine QML-singleton or for a regular object
	ENSURE(!this->engine || this->engine == engine, "relinking GameControl to another engine is not supported and not caught properly");

	if (!this->engine) {
		this->engine = engine;

		// add handlers
		engine->register_drawhud_action(this);

		auto &action = Engine::get().get_action_manager();

		auto &global_input_context = engine->get_input_manager().get_global_context();
		global_input_context.bind(action.get("TOGGLE_CONSTRUCT_MODE"), [this](const input::action_arg_t &) {
			this->set_mode((this->active_mode_index + 1) % this->modes.size());
		});

		// Switching between players with the 1-8 keys
		auto bind_player_switch = [this, &global_input_context](input::action_t action, int player_index) {
			global_input_context.bind(action, [this, player_index](const input::action_arg_t &) {
				if (this->current_player != player_index) {
					this->current_player = player_index;
					this->announce_current_player_name();
				}
			});

		};

		bind_player_switch(action.get("SWITCH_TO_PLAYER_1"), 1);
		bind_player_switch(action.get("SWITCH_TO_PLAYER_2"), 2);
		bind_player_switch(action.get("SWITCH_TO_PLAYER_3"), 3);
		bind_player_switch(action.get("SWITCH_TO_PLAYER_4"), 4);
		bind_player_switch(action.get("SWITCH_TO_PLAYER_5"), 5);
		bind_player_switch(action.get("SWITCH_TO_PLAYER_6"), 6);
		bind_player_switch(action.get("SWITCH_TO_PLAYER_7"), 7);
		bind_player_switch(action.get("SWITCH_TO_PLAYER_8"), 8);

		this->engine->announce_global_binds();
	}
}

void GameControl::set_game(GameMainHandle *game) {
	if (this->game != game) {
		if (this->game)
			QObject::disconnect(&this->game->gui_signals, &GameMainSignals::game_running, &this->gui_signals, &GameControlSignals::on_game_running);

		this->game = game;

		if (this->game)
			QObject::connect(&this->game->gui_signals, &GameMainSignals::game_running, &this->gui_signals, &GameControlSignals::on_game_running);
	}
}

void GameControl::set_modes(const std::vector<OutputMode*> &modes) {
	const int old_mode_index = this->active_mode_index;

	this->set_mode(-1);

	this->modes = modes;

	for (auto mode : this->modes)
		mode->set_game_control(this);

	if (old_mode_index != -1 && old_mode_index < std::distance(std::begin(this->modes), std::end(this->modes)))
		this->set_mode(old_mode_index);

	emit this->gui_signals.modes_changed(this->active_mode, this->active_mode_index);
}

void GameControl::announce_mode() {
	emit this->gui_signals.mode_changed(this->active_mode, this->active_mode_index);
}

void GameControl::announce_current_player_name() {
	if (Player *player = this->get_current_player()) {
		emit this->gui_signals.current_player_name_changed("[" + std::to_string(player->color) + "] " + player->name);
		emit this->gui_signals.current_civ_index_changed(player->civ->civ_id);
	}
}

bool GameControl::on_drawhud() {
	// render the active mode
	if (this->active_mode)
		this->active_mode->render();

	return true;
}

Player* GameControl::get_current_player() const {
	if (auto game = this->engine->get_game()) {
		unsigned int number = game->players.size();
		return &game->players[this->current_player % number];
	}
	return nullptr;
}

void GameControl::set_mode(int mode_index) {
	if (mode_index != -1) {
		if (mode_index < std::distance(std::begin(this->modes), std::end(this->modes)) &&this->modes[mode_index]->available()) {
			engine->get_input_manager().remove_context(this->active_mode);

			// set the new active mode
			if (this->active_mode) {
				this->active_mode->on_exit();
			}
			this->active_mode_index = mode_index;
			this->active_mode = this->modes[mode_index];
			this->active_mode->on_enter();

			// update the context
			engine->get_input_manager().register_context(this->active_mode);

			emit this->gui_signals.mode_changed(this->active_mode, this->active_mode_index);
		}
	} else {
		if (this->active_mode) {
			engine->get_input_manager().remove_context(this->active_mode);

			this->active_mode_index = -1;
			this->active_mode = nullptr;

			emit this->gui_signals.mode_changed(this->active_mode, this->active_mode_index);
		}
	}
}

} // openage
