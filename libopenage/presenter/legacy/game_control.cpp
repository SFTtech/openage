// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "game_control.h"

#include "error/error.h"
#include "gamestate/old/game_spec.h"
#include "legacy_engine.h"
#include "log/log.h"
#include "renderer/color.h"
#include "terrain/terrain_chunk.h"
#include "util/strings.h"


namespace openage {

OutputMode::OutputMode(qtsdl::GuiItemLink *gui_link) :
	game_control{nullptr},
	gui_link{gui_link} {
}

OutputMode::~OutputMode() {
	// An empty deconstructor prevents our clients from needing
	// to implement one.
	//
	// The deconstructor is needed in the first place to stop
	// the compiler from generating ud2 (undefined) instructors
	// for the body of this method.
}

void OutputMode::announce() {
	emit this->gui_signals.announced(this->name());

	emit this->gui_signals.binds_changed(this->active_binds());
}

void OutputMode::set_game_control(GameControl *game_control) {
	this->game_control = game_control;

	this->on_game_control_set();
	this->announce();
}

CreateMode::CreateMode(qtsdl::GuiItemLink *gui_link) :
	OutputMode{gui_link} {}

bool CreateMode::available() const {
	return true;
}

std::string CreateMode::name() const {
	return "Creation Mode";
}

void CreateMode::on_game_control_set() {}

void CreateMode::on_enter() {}

void CreateMode::on_exit() {}

void CreateMode::render() {}


ActionModeSignals::ActionModeSignals(ActionMode *action_mode) :
	action_mode(action_mode) {
}

void ActionModeSignals::on_action(const std::string &action_name) {
	presenter::LegacyDisplay *display = this->action_mode->game_control->get_display();

	const input::legacy::action_t &action = display->get_action_manager().get(action_name);
	input::legacy::InputContext *top_ctxt = &display->get_input_manager().get_top_context();

	if (top_ctxt == this->action_mode || top_ctxt == &this->action_mode->building_context || top_ctxt == &this->action_mode->build_menu_context || top_ctxt == &this->action_mode->build_menu_mil_context) {
		// create an action that just relays the action.
		input::legacy::action_arg_t action_arg{
			input::legacy::Event{input::legacy::event_class::ANY, 0, input::legacy::modset_t{}},
			coord::input{0, 0},
			coord::input_delta{0, 0},
			{action}};
		top_ctxt->execute_if_bound(action_arg);
	}
}

ActionMode::ActionMode(qtsdl::GuiItemLink *gui_link) :
	OutputMode{gui_link},
	selection{nullptr},
	use_set_ability{false},
	type_focus{nullptr},
	selecting{false},
	rng{rng::random_seed()},
	gui_signals{this} {}


void ActionMode::on_game_control_set() {
	ENSURE(this->game_control != nullptr, "no control was actually set");

	LegacyEngine *engine = this->game_control->get_engine();
	presenter::LegacyDisplay *display = this->game_control->get_display();
	ENSURE(engine != nullptr, "engine must be known!");

	auto &action = display->get_action_manager();
	auto input = &display->get_input_manager();
	coord::CoordManager &coord = display->coord;

	// TODO: the selection should not be used in here!
	this->selection = display->get_unit_selection();
	ENSURE(this->selection != nullptr, "selection must be fetched!");

	this->bind(action.get("TRAIN_OBJECT"),
	           [this](const input::legacy::action_arg_t &) {
				   // attempt to train editor selected object

				   // randomly select between male and female villagers
				   auto player = this->game_control->get_current_player();
				   auto type = player->get_type(this->rng.probability(0.5) ? 83 : 293);

				   Command cmd(*player, type);
				   cmd.add_flag(command_flag::interrupt);
				   this->selection->all_invoke(cmd);
			   });

	this->bind(action.get("ENABLE_BUILDING_PLACEMENT"),
	           [](const input::legacy::action_arg_t &) {
				   // this->building_placement = true;
			   });

	this->bind(action.get("DISABLE_SET_ABILITY"),
	           [this](const input::legacy::action_arg_t &) {
				   this->use_set_ability = false;
			   });

	this->bind(action.get("SET_ABILITY_MOVE"),
	           [this](const input::legacy::action_arg_t &) {
				   this->use_set_ability = true;
				   this->ability = ability_type::move;
				   emit this->gui_signals.ability_changed(std::to_string(this->ability));
			   });

	this->bind(action.get("SET_ABILITY_GATHER"),
	           [this](const input::legacy::action_arg_t &) {
				   this->use_set_ability = true;
				   this->ability = ability_type::gather;
				   emit this->gui_signals.ability_changed(std::to_string(this->ability));
			   });

	this->bind(action.get("SET_ABILITY_GARRISON"),
	           [this](const input::legacy::action_arg_t &) {
				   this->use_set_ability = true;
				   this->ability = ability_type::garrison;
				   emit this->gui_signals.ability_changed(std::to_string(this->ability));
			   });

	this->bind(action.get("SET_ABILITY_REPAIR"), [this](const input::legacy::action_arg_t &) {
		this->use_set_ability = true;
		this->ability = ability_type::repair;
		emit this->gui_signals.ability_changed(std::to_string(this->ability));
	});

	this->bind(action.get("SPAWN_VILLAGER"),
	           [this, display](const input::legacy::action_arg_t &) {
				   auto player = this->game_control->get_current_player();

				   if (player->type_count() > 0) {
					   UnitType &type = *player->get_type(590);

					   // TODO tile position
					   display->get_game()->placed_units.new_unit(type, *player, this->mousepos_phys3);
				   }
			   });

	this->bind(action.get("KILL_UNIT"),
	           [this](const input::legacy::action_arg_t &) {
				   this->selection->kill_unit(*this->game_control->get_current_player());
			   });

	this->bind(action.get("BUILD_MENU"),
	           [this, input](const input::legacy::action_arg_t &) {
				   log::log(MSG(dbg) << "Opening build menu");
				   input->push_context(&this->build_menu_context);
				   this->announce_buttons_type();
			   });

	this->bind(action.get("BUILD_MENU_MIL"),
	           [this, input](const input::legacy::action_arg_t &) {
				   log::log(MSG(dbg) << "Opening military build menu");
				   input->push_context(&this->build_menu_mil_context);
				   this->announce_buttons_type();
			   });

	this->build_menu_context.bind(action.get("CANCEL"),
	                              [this, input](const input::legacy::action_arg_t &) {
									  input->remove_context(&this->build_menu_context);
									  this->announce_buttons_type();
								  });

	this->build_menu_mil_context.bind(action.get("CANCEL"),
	                                  [this, input](const input::legacy::action_arg_t &) {
										  input->remove_context(&this->build_menu_mil_context);
										  this->announce_buttons_type();
									  });

	// Villager build commands
	auto bind_building_key = [this, input](input::legacy::action_t action, int building, input::legacy::InputContext *ctxt) {
		ctxt->bind(action, [this, building, ctxt, input](const input::legacy::action_arg_t &) {
			auto player = this->game_control->get_current_player();
			if (this->selection->contains_builders(*player)) {
				auto player = this->game_control->get_current_player();
				this->type_focus = player->get_type(building);

				if (player->can_make(*this->type_focus)) {
					if (&input->get_top_context() != &this->building_context) {
						input->remove_context(ctxt);
						input->push_context(&this->building_context);
						this->announce_buttons_type();
					}
				}
				else {
					// TODO show in game error message
				}
			}
		});
	};

	bind_building_key(action.get("BUILDING_HOUS"), 70, &this->build_menu_context); // House
	bind_building_key(action.get("BUILDING_MILL"), 68, &this->build_menu_context); // Mill
	bind_building_key(action.get("BUILDING_MINE"), 584, &this->build_menu_context); // Mining Camp
	bind_building_key(action.get("BUILDING_SMIL"), 562, &this->build_menu_context); // Lumber Camp
	bind_building_key(action.get("BUILDING_DOCK"), 47, &this->build_menu_context); // Dock
	// TODO: Doesn't show until it is placed
	bind_building_key(action.get("BUILDING_FARM"), 50, &this->build_menu_context); // Farm
	bind_building_key(action.get("BUILDING_BLAC"), 103, &this->build_menu_context); // Blacksmith
	bind_building_key(action.get("BUILDING_MRKT"), 84, &this->build_menu_context); // Market
	bind_building_key(action.get("BUILDING_CRCH"), 104, &this->build_menu_context); // Monastery
	bind_building_key(action.get("BUILDING_UNIV"), 209, &this->build_menu_context); // University
	bind_building_key(action.get("BUILDING_RTWC"), 109, &this->build_menu_context); // Town Center
	bind_building_key(action.get("BUILDING_WNDR"), 276, &this->build_menu_context); // Wonder

	bind_building_key(action.get("BUILDING_BRKS"), 12, &this->build_menu_mil_context); // Barracks
	bind_building_key(action.get("BUILDING_ARRG"), 87, &this->build_menu_mil_context); // Archery Range
	bind_building_key(action.get("BUILDING_STBL"), 101, &this->build_menu_mil_context); // Stable
	bind_building_key(action.get("BUILDING_SIWS"), 49, &this->build_menu_mil_context); // Siege Workshop
	bind_building_key(action.get("BUILDING_WCTWX"), 598, &this->build_menu_mil_context); // Outpost
	// TODO for palisade and stone wall: Drag walls, automatically adjust orientation
	// TODO: This just cycles through all palisade textures
	bind_building_key(action.get("BUILDING_WALL"), 72, &this->build_menu_mil_context); // Palisade Wall
	// TODO: Fortified wall has a different ID
	bind_building_key(action.get("BUILDING_WALL2"), 117, &this->build_menu_mil_context); // Stone Wall
	// TODO: Upgraded versions have different IDs
	bind_building_key(action.get("BUILDING_WCTW"), 79, &this->build_menu_mil_context); // Watch Tower
	bind_building_key(action.get("BUILDING_WCTW4"), 236, &this->build_menu_mil_context); // Bombard Tower
	// TODO: Gate placement - 659 is horizontal closed
	bind_building_key(action.get("BUILDING_GTCA2"), 659, &this->build_menu_mil_context); // Gate
	bind_building_key(action.get("BUILDING_CSTL"), 82, &this->build_menu_mil_context); // Castle

	this->building_context.bind(action.get("CANCEL"),
	                            [this, input](const input::legacy::action_arg_t &) {
									input->remove_context(&this->building_context);
									this->announce_buttons_type();
									this->type_focus = nullptr;
								});

	auto bind_build = [this, input, &coord](input::legacy::action_t action, const bool increase) {
		this->building_context.bind(action, [this, increase, input, &coord](const input::legacy::action_arg_t &arg) {
			this->mousepos_phys3 = arg.mouse.to_phys3(coord, 0);
			this->mousepos_tile = this->mousepos_phys3.to_tile();

			bool placed = this->place_selection(this->mousepos_phys3);

			if (placed && !increase) {
				this->type_focus = nullptr;
				input->remove_context(&this->building_context);
				this->announce_buttons_type();
			}
		});
	};

	bind_build(action.get("BUILD"), false);
	bind_build(action.get("KEEP_BUILDING"), true);

	auto bind_select = [this, input, display](input::legacy::action_t action, const bool increase) {
		this->bind(action, [this, increase, input, display](const input::legacy::action_arg_t &arg) {
			auto mousepos_camgame = arg.mouse.to_camgame(display->coord);
			Terrain *terrain = display->get_game()->terrain.get();

			this->selection->drag_update(mousepos_camgame);
			this->selection->drag_release(*this->game_control->get_current_player(), terrain, increase);
			InputContext *top_ctxt = &input->get_top_context();

			if ((this->selection->get_selection_type() != selection_type_t::own_units or this->selection->contains_military(*this->game_control->get_current_player()))
			    and (top_ctxt == &this->build_menu_context || top_ctxt == &this->build_menu_mil_context)) {
				input->remove_context(top_ctxt);
			}
			this->announce_buttons_type();
			this->announce_current_selection();
		});
	};

	bind_select(action.get("SELECT"), false);
	bind_select(action.get("INCREASE_SELECTION"), true);

	this->bind(action.get("ORDER_SELECT"), [this, input, &coord](const input::legacy::action_arg_t &arg) {
		if (this->type_focus) {
			// right click can cancel building placement
			this->type_focus = nullptr;
			input->remove_context(&this->building_context);
			this->announce_buttons_type();
		}

		auto cmd = this->get_action(arg.mouse.to_phys3(coord));
		cmd.add_flag(command_flag::interrupt);
		this->selection->all_invoke(cmd);
		this->use_set_ability = false;
	});

	this->bind(action.get("BEGIN_SELECTION"), [this](const input::legacy::action_arg_t &) {
		this->selecting = true;
	});

	this->bind(action.get("END_SELECTION"), [this](const input::legacy::action_arg_t &) {
		this->selecting = false;
	});

	this->bind(input::legacy::event_class::MOUSE, [this, &coord](const input::legacy::action_arg_t &arg) {
		auto mousepos_camgame = arg.mouse.to_camgame(coord);
		this->mousepos_phys3 = mousepos_camgame.to_phys3(coord);
		this->mousepos_tile = this->mousepos_phys3.to_tile();

		// drag selection box
		if (arg.e.cc == input::legacy::ClassCode(input::legacy::event_class::MOUSE_MOTION, 0) && this->selecting && !this->type_focus) {
			this->selection->drag_update(mousepos_camgame);
			this->announce_current_selection();
			return true;
		}
		return false;
	});
}

bool ActionMode::available() const {
	if (this->game_control == nullptr) {
		log::log(MSG(warn) << "ActionMode::available() queried without "
		                      "game control being attached.");
		return false;
	}

	presenter::LegacyDisplay *display = this->game_control->get_display();
	if (display->get_game() != nullptr) {
		return true;
	}
	else {
		log::log(MSG(warn) << "Cannot enter action mode without a game");
		return false;
	}
}

void ActionMode::on_enter() {}

void ActionMode::on_exit() {
	// Since on_exit is called after removing the active mode, if the top context isn't the global one then it must
	// be either a build menu or the building context
	presenter::LegacyDisplay *display = this->game_control->get_display();
	auto *input_manager = &display->get_input_manager();
	InputContext *top_ctxt = &input_manager->get_top_context();
	if (top_ctxt != &input_manager->get_global_context()) {
		if (top_ctxt == &this->building_context) {
			this->type_focus = nullptr;
		}
		input_manager->remove_context(top_ctxt);
		this->announce_buttons_type();
	}
	this->selecting = false;
}

Command ActionMode::get_action(const coord::phys3 &pos) const {
	presenter::LegacyDisplay *display = this->game_control->get_display();
	GameMain *game = display->get_game();

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
		auto player = this->game_control->get_current_player();

		if (player->can_make(*this->type_focus)) {
			// confirm building placement with left click
			// first create foundation using the producer
			presenter::LegacyDisplay *display = this->game_control->get_display();

			UnitContainer *container = &display->get_game()->placed_units;
			UnitReference new_building = container->new_unit(*this->type_focus, *player, point);

			// task all selected villagers to build
			// TODO: editor placed objects are completed already
			if (new_building.is_valid()) {
				player->deduct(this->type_focus->cost.get(*player)); // TODO change, move elsewheres

				Command cmd(*player, new_building.get());
				cmd.set_ability(ability_type::build);
				cmd.add_flag(command_flag::interrupt);
				this->selection->all_invoke(cmd);
				return true;
			}
		}
		else {
			// TODO show in game error message
			return false;
		}
	}
	return false;
}

void ActionMode::render() {
	ENSURE(this->game_control != nullptr, "game_control is unset");
	LegacyEngine *engine = this->game_control->get_engine();
	presenter::LegacyDisplay *display = this->game_control->get_display();

	ENSURE(engine != nullptr, "engine is needed to render ActionMode");

	if (GameMain *game = display->get_game()) {
		Player *player = this->game_control->get_current_player();

		this->announce_resources();

		if (this->selection && this->selection->get_units_count() > 0) {
			this->announce_current_selection();
		}

		// when a building is being placed
		if (this->type_focus) {
			auto txt = this->type_focus->default_texture();
			auto size = this->type_focus->foundation_size;

			tile_range center = building_center(this->mousepos_phys3, size, *game->terrain);
			txt->sample(display->coord, center.draw.to_camhud(display->coord), player->color);
		}
	}
	else {
		display->render_text({0, 140}, 12, renderer::Colors::WHITE, "Action Mode requires a game");
	}

	ENSURE(this->selection != nullptr, "selection not set");

	this->selection->on_drawhud();
}

std::string ActionMode::name() const {
	return "Action Mode";
}

void ActionMode::announce() {
	this->OutputMode::announce();

	this->announce_resources();
	emit this->gui_signals.ability_changed(
		this->use_set_ability ? std::to_string(this->ability) : "");
}

void ActionMode::announce_resources() {
	if (this->game_control) {
		if (Player *player = this->game_control->get_current_player()) {
			for (auto i = static_cast<std::underlying_type<game_resource>::type>(game_resource::RESOURCE_TYPE_COUNT); i != 0; --i) {
				auto resource_type = static_cast<game_resource>(i - 1);

				emit this->gui_signals.resource_changed(
					resource_type,
					static_cast<int>(player->amount(resource_type)));
			}
			emit this->gui_signals.population_changed(player->population.get_demand(), player->population.get_capacity(), player->population.get_space() <= 0 && !player->population.is_capacity_maxed());
		}
	}
}

void ActionMode::announce_buttons_type() {
	ActionButtonsType buttons_type;
	presenter::LegacyDisplay *display = this->game_control->get_display();
	InputContext *top_ctxt = &display->get_input_manager().get_top_context();
	if (top_ctxt == &this->build_menu_context) {
		buttons_type = ActionButtonsType::BuildMenu;
	}
	else if (top_ctxt == &this->build_menu_mil_context) {
		buttons_type = ActionButtonsType::MilBuildMenu;
	}
	else if (top_ctxt == &this->building_context || this->selection->get_selection_type() != selection_type_t::own_units) {
		buttons_type = ActionButtonsType::None;
	}
	else if (this->selection->contains_military(*this->game_control->get_current_player())) {
		buttons_type = ActionButtonsType::MilitaryUnits;
	}
	else {
		buttons_type = ActionButtonsType::CivilianUnits;
	}

	if (buttons_type != this->buttons_type) {
		this->buttons_type = buttons_type;
		emit this->gui_signals.buttons_type_changed(buttons_type);

		// announce the changed input context
		this->announce();
	}
}

EditorModeSignals::EditorModeSignals(EditorMode *editor_mode) :
	editor_mode{editor_mode} {
}

void EditorModeSignals::on_current_player_name_changed() {
	this->editor_mode->announce_categories();
}

EditorMode::EditorMode(qtsdl::GuiItemLink *gui_link) :
	OutputMode{gui_link},
	editor_current_terrain{-1},
	current_type_id{-1},
	paint_terrain{},
	gui_signals{this} {}


void EditorMode::on_game_control_set() {
	presenter::LegacyDisplay *display = this->game_control->get_display();
	auto &action = display->get_action_manager();

	// bind required hotkeys
	this->bind(action.get("ENABLE_BUILDING_PLACEMENT"), [this](const input::legacy::action_arg_t &) {
		log::log(MSG(dbg) << "change category");
		emit this->gui_signals.toggle();
	});

	this->bind(input::legacy::event_class::MOUSE, [this, display](const input::legacy::action_arg_t &arg) {
		if (arg.e.cc == input::legacy::ClassCode(input::legacy::event_class::MOUSE_BUTTON, 1) || display->get_input_manager().is_down(input::legacy::event_class::MOUSE_BUTTON, 1)) {
			if (this->paint_terrain) {
				this->paint_terrain_at(arg.mouse.to_viewport(display->coord));
			}
			else {
				this->paint_entity_at(arg.mouse.to_viewport(display->coord), false);
			}
			return true;
		}
		else if (arg.e.cc == input::legacy::ClassCode(input::legacy::event_class::MOUSE_BUTTON, 3) || display->get_input_manager().is_down(input::legacy::event_class::MOUSE_BUTTON, 3)) {
			if (!this->paint_terrain) {
				this->paint_entity_at(arg.mouse.to_viewport(display->coord), true);
			}
			return true;
		}
		return false;
	});
}

bool EditorMode::available() const {
	if (this->game_control == nullptr) {
		log::log(MSG(warn) << "game_control not yet linked to EditorMode");
		return false;
	}
	else if (this->game_control->get_display()->get_game() != nullptr) {
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

void EditorMode::paint_terrain_at(const coord::viewport &point) {
	presenter::LegacyDisplay *display = this->game_control->get_display();
	Terrain *terrain = display->get_game()->terrain.get();

	auto mousepos_tile = point.to_tile(display->coord);

	TerrainChunk *chunk = terrain->get_create_chunk(mousepos_tile);
	chunk->get_data(mousepos_tile.get_pos_on_chunk())->terrain_id = editor_current_terrain;
}

void EditorMode::paint_entity_at(const coord::viewport &point, const bool del) {
	presenter::LegacyDisplay *display = this->game_control->get_display();
	GameMain *game = display->get_game();
	Terrain *terrain = game->terrain.get();

	auto mousepos_phys3 = point.to_phys3(display->coord);
	auto mousepos_tile = mousepos_phys3.to_tile();

	TerrainChunk *chunk = terrain->get_create_chunk(mousepos_tile);
	// TODO : better detection of presence of unit
	if (!chunk->get_data(mousepos_tile.get_pos_on_chunk())->obj.empty()) {
		if (del) {
			// delete first object currently standing at the clicked position
			TerrainObject *obj = chunk->get_data(mousepos_tile.get_pos_on_chunk())->obj[0];
			obj->remove();
		}
	}
	else if (!del && this->current_type_id != -1) {
		Player *player = this->game_control->get_current_player();
		UnitType *selected_type = player->get_type(this->current_type_id);

		// tile is empty so try creating a unit
		UnitContainer *container = &game->placed_units;
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

			std::for_each(std::begin(inds), std::end(inds), [player, &it](auto index) {
				*it++ = std::make_tuple(index, player->get_type(index)->default_texture()->id);
			});

			emit this->gui_signals.category_content_changed(category_name, type_and_texture);
		}
}

void EditorMode::announce() {
	OutputMode::announce();
	this->announce_categories();
}

void EditorMode::set_game_control(GameControl *game_control) {
	if (this->game_control != game_control) {
		if (this->game_control)
			QObject::disconnect(
				&this->game_control->gui_signals,
				&GameControlSignals::current_player_name_changed,
				&this->gui_signals,
				&EditorModeSignals::on_current_player_name_changed);

		// actually set the control
		OutputMode::set_game_control(game_control);

		if (this->game_control)
			QObject::connect(
				&this->game_control->gui_signals,
				&GameControlSignals::current_player_name_changed,
				&this->gui_signals,
				&EditorModeSignals::on_current_player_name_changed);

		this->announce_categories();
	}
	else {
		// just set the control
		OutputMode::set_game_control(game_control);
	}
}

GameControlSignals::GameControlSignals(GameControl *game_control) :
	game_control{game_control} {
}

void GameControlSignals::on_game_running(bool running) {
	if (running)
		this->game_control->announce_current_player_name();
}

GameControl::GameControl(qtsdl::GuiItemLink *gui_link) :
	engine{nullptr},
	game{nullptr},
	active_mode{nullptr},
	active_mode_index{-1},
	current_player{1},
	gui_signals{this},
	gui_link{gui_link} {
}

void GameControl::set_engine(LegacyEngine *engine) {
	// TODO: decide to either go for a full Engine QML-singleton or for a regular object
	ENSURE(!this->engine || this->engine == engine, "relinking GameControl to another engine is not supported and not caught properly");

	if (not this->engine) {
		this->engine = engine;

		// add handlers
		this->display->register_drawhud_action(this, -1);

		auto &action = this->display->get_action_manager();

		auto &global_input_context = display->get_input_manager().get_global_context();

		// advance the active mode
		global_input_context.bind(action.get("TOGGLE_CONSTRUCT_MODE"), [this](const input::legacy::action_arg_t &) {
			this->set_mode((this->active_mode_index + 1) % this->modes.size());
		});

		// Switching between players with the 1-8 keys
		auto bind_player_switch = [this, &global_input_context](input::legacy::action_t action, size_t player_index) {
			global_input_context.bind(action, [this, player_index](const input::legacy::action_arg_t &) {
				if (this->current_player != player_index)
					if (auto game = this->display->get_game())
						if (player_index < game->player_count()) {
							this->current_player = player_index;
							this->announce_current_player_name();
						}
			});
		};

		bind_player_switch(action.get("SWITCH_TO_PLAYER_1"), 0);
		bind_player_switch(action.get("SWITCH_TO_PLAYER_2"), 1);
		bind_player_switch(action.get("SWITCH_TO_PLAYER_3"), 2);
		bind_player_switch(action.get("SWITCH_TO_PLAYER_4"), 3);
		bind_player_switch(action.get("SWITCH_TO_PLAYER_5"), 4);
		bind_player_switch(action.get("SWITCH_TO_PLAYER_6"), 5);
		bind_player_switch(action.get("SWITCH_TO_PLAYER_7"), 6);
		bind_player_switch(action.get("SWITCH_TO_PLAYER_8"), 7);

		this->display->announce_global_binds();
	}
}
void GameControl::set_display(presenter::LegacyDisplay *display) {
	this->display = display;
}

void GameControl::set_game(GameMainHandle *game) {
	if (this->game != game) {
		if (this->game)
			QObject::disconnect(&this->game->gui_signals,
			                    &GameMainSignals::game_running,
			                    &this->gui_signals,
			                    &GameControlSignals::on_game_running);

		this->game = game;

		if (this->game)
			QObject::connect(&this->game->gui_signals,
			                 &GameMainSignals::game_running,
			                 &this->gui_signals,
			                 &GameControlSignals::on_game_running);
	}
}

void GameControl::set_modes(const std::vector<OutputMode *> &modes) {
	const int old_mode_index = this->active_mode_index;

	this->set_mode(-1);
	this->modes = modes;

	for (auto mode : this->modes) {
		// link the controller to the mode
		mode->set_game_control(this);
	}

	// announce the newly set modes
	emit this->gui_signals.modes_changed(this->active_mode,
	                                     this->active_mode_index);

	// try to enter the mode we were in before
	// assuming its index didn't change.
	if (old_mode_index != -1) {
		if (old_mode_index < std::distance(std::begin(this->modes),
		                                   std::end(this->modes))) {
			this->set_mode(old_mode_index, true);
		}
		else {
			log::log(MSG(warn) << "couldn't enter previous gui mode #"
			                   << old_mode_index << " as it vanished.");
		}
	}
}

void GameControl::announce_mode() {
	emit this->gui_signals.mode_changed(this->active_mode,
	                                    this->active_mode_index);

	if (this->active_mode != nullptr) {
		emit this->active_mode->announce();
	}
}

void GameControl::announce_current_player_name() {
	if (Player *player = this->get_current_player()) {
		emit this->gui_signals.current_player_name_changed(
			"[" + std::to_string(player->color) + "] " + player->name);
		emit this->gui_signals.current_civ_index_changed(player->civ->civ_id);
	}
}

void ActionMode::announce_current_selection() {
	Player *player = this->game_control->get_current_player();
	emit this->gui_signals.selection_changed(this->selection, player);
}

bool GameControl::on_drawhud() {
	// render the active mode
	if (this->active_mode)
		this->active_mode->render();

	return true;
}

Player *GameControl::get_current_player() const {
	if (auto game = this->display->get_game()) {
		unsigned int number = game->player_count();
		return game->get_player(this->current_player % number);
	}
	return nullptr;
}

void GameControl::set_mode(int mode_index, bool signal_if_unchanged) {
	bool need_to_signal = signal_if_unchanged;

	// do we wanna set a new mode?
	if (mode_index != -1) {
		// if the new mode is valid, available and not active at the moment
		if (mode_index < std::distance(std::begin(this->modes),
		                               std::end(this->modes))
		    && this->modes[mode_index]->available()
		    && this->active_mode_index != mode_index) {
			ENSURE(!this->active_mode == (this->active_mode_index == -1),
			       "inconsistency between the active mode index and pointer");

			// exit from the old mode
			if (this->active_mode) {
				this->display->get_input_manager().remove_context(
					this->active_mode);

				// trigger the exit callback of the mode
				if (this->active_mode) {
					this->active_mode->on_exit();
				}
			}

			// set the new active mode
			this->active_mode_index = mode_index;
			this->active_mode = this->modes[mode_index];

			// trigger the entry callback
			this->active_mode->on_enter();

			// add the mode-local input context
			this->display->get_input_manager().push_context(
				this->active_mode);

			need_to_signal = true;
		}
	}
	else {
		// unassign the active mode
		if (this->active_mode) {
			// remove the input context
			this->display->get_input_manager().remove_context(
				this->active_mode);

			this->active_mode_index = -1;
			this->active_mode = nullptr;

			need_to_signal = true;
		}
	}

	if (need_to_signal) {
		this->announce_mode();
	}
}

LegacyEngine *GameControl::get_engine() const {
	if (this->engine == nullptr) {
		throw Error{MSG(err) << "game control doesn't have a valid engine pointer yet"};
	}

	return this->engine;
}

presenter::LegacyDisplay *GameControl::get_display() const {
	if (this->engine == nullptr) {
		throw Error{MSG(err) << "game control doesn't have a valid engine pointer yet"};
	}

	return this->display;
}


} // namespace openage
