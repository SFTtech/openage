// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GAME_CONTROL_H_
#define OPENAGE_GAME_CONTROL_H_

#include "coord/camgame.h"
#include "input/input_context.h"
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
	 * used when switching modes
	 */
	virtual void on_enter() = 0;
	virtual void render() = 0;

};

/**
 * game creation menu
 */
class CreateMode : public OutputMode {
public:
	CreateMode(Engine &engine);

	void on_enter() override;
	void render() override;

private:
	game_settings settings;

	// access to games asset files
	AssetManager assetmanager;

};

/**
 * Control units and issue commands
 */
class ActionMode : public OutputMode {
public:
	ActionMode();

	void on_enter() override;
	void render() override;

	bool on_mouse_wheel(int direction, coord::window point);
	bool on_single_click(int button, coord::window point);

private:

	// currently selected units
	UnitSelection selection;

	// restrict command abilities
	bool use_set_ability;
	ability_type ability;

	// a selected type
	UnitType *type_focus;
	coord::tile mousepos_tile;

	/**
	 * decides which type of right mouse click command to issue based on position
	 *
	 * if a unit is at the position the command should target the unit,
	 * otherwise target ground position
	 */
	Command get_action(const coord::phys3 &pos) const;

};

/**
 * editor mode provides an interface for map editing
 */
class EditorMode : public OutputMode {
public:
	EditorMode();

	void on_enter() override;
	void render() override;

	bool on_mouse_wheel(int direction, coord::window point);
	bool on_single_click(int button, coord::window point);
	bool on_drag_start(int button, coord::window point);
	bool on_drag_end(int button, coord::window point);

private:

	// currently selected terrain id
	openage::terrain_t editor_current_terrain;
	int editor_current_building;

	// TODO assign placement to player object
	bool paint_terrain;
	bool building_placement;

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

} //namespace openage

#endif
