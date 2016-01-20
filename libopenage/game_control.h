// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "coord/camgame.h"
#include "input/input_context.h"
#include "rng/rng.h"
#include "unit/command.h"
#include "unit/selection.h"
#include "unit/unit_type.h"
#include "engine.h"
#include "game_main.h"
#include "handlers.h"

namespace openage {

/**
 * a target for input handling and gui rendering
 */
class OutputMode : public input::InputContext {
public:

	/**
	 * is this mode able to be used
	 */
	virtual bool available() const = 0;

	/**
	 * used when switching modes
	 */
	virtual void on_enter() = 0;
	virtual void render() = 0;

	/**
	 * Used for displaying the current mode.
	 */
	virtual std::string name() const = 0;
};

/**
 * game creation menu
 */
class CreateMode : public OutputMode {
public:
	CreateMode();

	bool available() const override;
	void on_enter() override;
	void render() override;
	std::string name() const override;

private:
	int selected;

	bool setting_value;
	std::string new_value;
	std::string response_value;

};

/**
 * Control units and issue commands
 */
class ActionMode : public OutputMode {
public:
	ActionMode();

	bool available() const override;
	void on_enter() override;
	void render() override;
	std::string name() const override;

	bool on_mouse_wheel(int direction, coord::window point);
	bool on_single_click(int button, coord::window point);

private:

	/**
	 * decides which type of right mouse click command to issue based on position
	 *
	 * if a unit is at the position the command should target the unit,
	 * otherwise target ground position
	 */
	Command get_action(const coord::phys3 &pos) const;


	/**
	 * places hovering building
	 */
	bool place_selection(coord::phys3 point);

	// currently selected units
	UnitSelection selection;

	// restrict command abilities
	bool use_set_ability;
	ability_type ability;

	// a selected type for placement
	UnitType *type_focus;
	coord::phys3 mousepos_phys3;
	coord::tile mousepos_tile;

	// used for random type creation
	rng::RNG rng;
};

/**
 * editor mode provides an interface for map editing
 */
class EditorMode : public OutputMode {
public:
	EditorMode();

	bool available() const override;
	void on_enter() override;
	void render() override;
	std::string name() const override;

	bool on_mouse_wheel(int direction, coord::window point);
	bool on_single_click(int button, coord::window point);

private:

	// currently selected terrain id
	openage::terrain_t editor_current_terrain;
	unsigned int editor_current_type;
	unsigned int editor_category;
	UnitType *selected_type;
	std::string category;

	// true = terrain painting, false = unit placement
	bool paint_terrain;

};


/**
 * connects the gui system with the game engine
 * switches between contexts such as editor mode and
 * action mode
 *
 * hud rendering and input handling is redirected to the active mode
 */
class GameControl :
		public openage::HudHandler {
public:
	GameControl(openage::Engine *engine);

	void toggle_mode();

	bool on_drawhud() override;

private:
	Engine *engine;

	// control modes
	std::vector<std::unique_ptr<OutputMode>> modes;

	OutputMode *active_mode;
	int active_mode_index;

};

} // openage
