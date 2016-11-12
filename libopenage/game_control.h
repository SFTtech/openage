// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <tuple>

#include <QObject>

#include "coord/camgame.h"
#include "input/input_context.h"
#include "rng/rng.h"
#include "gamestate/game_main.h"
#include "unit/command.h"
#include "unit/selection.h"
#include "unit/unit_type.h"
#include "engine.h"
#include "handlers.h"

namespace qtsdl {
class GuiItemLink;
} // qtsdl

namespace openage {

class GameControl;

class OutputModeSignals : public QObject {
	Q_OBJECT

public:
signals:
	void announced(const std::string &name, const std::vector<std::string>& binds);
};

/**
 * a target for input handling and gui rendering
 */
class OutputMode : public input::InputContext {
public:
	explicit OutputMode(qtsdl::GuiItemLink *gui_link);

	/**
	 * is this mode able to be used
	 */
	virtual bool available() const = 0;

	/**
	 * used when switching modes
	 */
	virtual void on_enter() = 0;
	virtual void on_exit() = 0;
	virtual void render() = 0;

	/**
	 * Used for displaying the current mode.
	 */
	virtual std::string name() const = 0;

	virtual void announce();

	virtual void set_game_control(GameControl *game_control);

protected:
	GameControl *game_control;

public:
	OutputModeSignals gui_signals;
	qtsdl::GuiItemLink *gui_link;
};

/**
 * game creation menu
 */
class CreateMode : public OutputMode {
public:
	CreateMode(qtsdl::GuiItemLink *gui_link);

	bool available() const override;
	void on_enter() override;
	void on_exit() override;
	void render() override;
	std::string name() const override;
};

enum class ActionButtonsType {
	None,
	MilitaryUnits,
	CivilianUnits,
	BuildMenu,
	MilBuildMenu
};

class ActionMode;

class ActionModeSignals : public QObject {
	Q_OBJECT

public:
	explicit ActionModeSignals(ActionMode *action_mode);

public slots:
	void on_action(const std::string &action_name);

signals:
	void resource_changed(game_resource resource, int amount);
	void ability_changed(const std::string &ability);
	void buttons_type_changed(const ActionButtonsType type);

private:
	ActionMode *action_mode;
};

/**
 * Control units and issue commands
 */
class ActionMode : public OutputMode {
public:
	ActionMode(qtsdl::GuiItemLink *gui_link);

	bool available() const override;
	void on_enter() override;
	void on_exit() override;
	void render() override;
	std::string name() const override;

private:
	friend ActionModeSignals;
	/**
	 * sends to gui the properties that it needs
	 */
	virtual void announce() override;

	/**
	 * sends to gui the amounts of resources
	 */
	void announce_resources();

	/**
	 * sends to gui the buttons it should use for the action buttons (if changed)
	 */
	void announce_buttons_type();

	/**
	 * decides which type of right mouse click command to issue based on position
	 *
	 * if a unit is at the position the command should target the unit,
	 * otherwise target ground position
	 */
	Command get_action(const coord::phys3 &pos) const;

	/**
	 * used after opening the build menu
	 */
	InputContext build_menu_context;

	/**
	 * used after opening the military build menu
	 */
	InputContext build_menu_mil_context;

	/**
	 * used when selecting the building placement
	 */
	InputContext building_context;

	/**
	 * places hovering building
	 */
	bool place_selection(coord::phys3 point);

	// currently selected units
	UnitSelection *selection;

	// restrict command abilities
	bool use_set_ability;
	ability_type ability;

	// a selected type for placement
	UnitType *type_focus;
	coord::phys3 mousepos_phys3;
	coord::tile mousepos_tile;
	bool selecting;

	ActionButtonsType buttons_type;

	// used for random type creation
	rng::RNG rng;

public:
	ActionModeSignals gui_signals;
};

class EditorMode;

class EditorModeSignals : public QObject {
	Q_OBJECT

public:
	explicit EditorModeSignals(EditorMode *editor_mode);

public slots:
	void on_current_player_name_changed();

signals:
	void toggle();
	void categories_changed(const std::vector<std::string> &categories);
	void categories_content_changed();
	void category_content_changed(const std::string &category_name, std::vector<std::tuple<index_t, uint16_t>> &type_and_texture);

private:
	EditorMode *editor_mode;
};

/**
 * editor mode provides an interface for map editing
 */
class EditorMode : public OutputMode {
public:
	explicit EditorMode(qtsdl::GuiItemLink *gui_link);

	bool available() const override;
	void on_enter() override;
	void on_exit() override;
	void render() override;
	std::string name() const override;

	void set_current_type_id(int current_type_id);
	void set_current_terrain_id(openage::terrain_t current_terrain_id);
	void set_paint_terrain(bool paint_terrain);

	bool on_single_click(int button, coord::window point);

	void announce_categories();
	void announce_category_content(const std::string &category_name);

private:
	virtual void announce() override;
	virtual void set_game_control(GameControl *game_control) override;

	void paint_terrain_at(const coord::window &point);
	void paint_entity_at(const coord::window &point, const bool del);
	// currently selected terrain id
	openage::terrain_t editor_current_terrain;
	int current_type_id;
	std::string category;

	// true = terrain painting, false = unit placement
	bool paint_terrain;

public:
	EditorModeSignals gui_signals;

};

class GameControl;

class GameControlSignals : public QObject {
	Q_OBJECT

public:
	explicit GameControlSignals(GameControl *game_control);

public slots:
	void on_game_running(bool running);

signals:
	void mode_changed(OutputMode *mode, int mode_index);
	void modes_changed(OutputMode *mode, int mode_index);

	void current_player_name_changed(const std::string &current_player_name);
	void current_civ_index_changed(int current_civ_index);

private:
	GameControl *game_control;
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
	explicit GameControl(qtsdl::GuiItemLink *gui_link);

	void set_engine(Engine *engine);
	void set_game(GameMainHandle *game);

	void set_modes(const std::vector<OutputMode*> &modes);

	void set_mode(int mode, bool signal_if_unchanged=false);
	void announce_mode();
	void announce_current_player_name();

	bool on_drawhud() override;

	Player* get_current_player() const;

private:
	Engine *engine;
	GameMainHandle *game;

	// control modes
	std::vector<OutputMode*> modes;

	OutputMode *active_mode;
	int active_mode_index;

	size_t current_player;

public:
	GameControlSignals gui_signals;
	qtsdl::GuiItemLink *gui_link;
};

} // openage
