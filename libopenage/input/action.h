// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_map>
#include <vector>

#include "../coord/window.h"
#include "event.h"

namespace openage {

class Engine;

namespace cvar {
class CVarManager;
} // cvar

namespace input {

class InputManager;

/**
 * Used to identify actions.
 */
using action_t = unsigned int;


/**
 * Mapping type from action name to action id.
 */
using action_map_t = std::unordered_map<std::string, action_t>;


/**
 * The action manager manages all the actions allow creation, access
 * information and equality.
 */
class ActionManager {
public:
	ActionManager(InputManager *input_manager,
	              cvar::CVarManager *cvar_manager);
	bool create(const std::string type);
	action_t get(const std::string &type);
	std::string get_name(const action_t action);
	bool is(const std::string &type, const action_t action);

private :
	const std::vector<std::string> default_action = {
		"UNDEFINED",
		"START_GAME",
		"STOP_GAME",
		"TOGGLE_HUD",
		"SCREENSHOT",
		"TOGGLE_DEBUG_OVERLAY",
		"TOGGLE_DEBUG_GRID",
		"QUICK_SAVE",
		"QUICK_LOAD",
		"TOGGLE_MODE",
		"TOGGLE_MENU",
		"TOGGLE_ITEM",
		"TOGGLE_BLENDING",
		"TOGGLE_FILLMODE",
		"TOGGLE_PROFILER",
		"TOGGLE_CONSTRUCT_MODE",
		"TOGGLE_UNIT_DEBUG",
		"TRAIN_OBJECT",
		"ENABLE_BUILDING_PLACEMENT",
		"DISABLE_SET_ABILITY",
		"SET_ABILITY_MOVE",
		"SET_ABILITY_GATHER",
		"SET_ABILITY_GARRISON",
		"TOGGLE_CONSOLE",
		"SPAWN_VILLAGER",
		"KILL_UNIT",
		"BUILD_MENU",
		"BUILD_MENU_MIL",
		"CANCEL",
		"BUILDING_HOUS",
		"BUILDING_MILL",
		"BUILDING_MINE",
		"BUILDING_SMIL",
		"BUILDING_DOCK",
		"BUILDING_FARM",
		"BUILDING_BLAC",
		"BUILDING_MRKT",
		"BUILDING_CRCH",
		"BUILDING_UNIV",
		"BUILDING_RTWC",
		"BUILDING_WNDR",
		"BUILDING_BRKS",
		"BUILDING_ARRG",
		"BUILDING_STBL",
		"BUILDING_SIWS",
		"BUILDING_WCTWX",
		"BUILDING_WALL",
		"BUILDING_WALL2",
		"BUILDING_WCTW",
		"BUILDING_WCTW4",
		"BUILDING_GTCA2",
		"BUILDING_CSTL",
		"BUILDING_TOWN_CENTER",
		"SWITCH_TO_PLAYER_1",
		"SWITCH_TO_PLAYER_2",
		"SWITCH_TO_PLAYER_3",
		"SWITCH_TO_PLAYER_4",
		"SWITCH_TO_PLAYER_5",
		"SWITCH_TO_PLAYER_6",
		"SWITCH_TO_PLAYER_7",
		"SWITCH_TO_PLAYER_8",
		"UP_ARROW",
		"DOWN_ARROW",
		"LEFT_ARROW",
		"RIGHT_ARROW",
		"SELECT",
		"DESELECT",
		"BEGIN_SELECTION",
		"END_SELECTION",
		"FORWARD",
		"BACK",
		"PAINT_TERRAIN",
		"BUILDING_5",
		"BUILDING_6",
		"BUILDING_7",
		"BUILDING_8",
		"BUILD",
		"KEEP_BUILDING",
		"INCREASE_SELECTION",
		"ORDER_SELECT"
	};

	action_map_t actions;

	InputManager *input_manager;
	cvar::CVarManager *cvar_manager;
};


// TODO: use action_hint_t = std::pair<action_t, int>
// for example a building command
// std::make_pair(action_t::BUILD, 123)
using action_id_t = action_t;


/**
 * Contains information about a triggered event.
 */
struct action_arg_t {

	// Triggering event
	const Event e;

	// Mouse position
	const coord::window mouse;
	const coord::window_delta motion;

	// hints for arg receiver
	// these get set globally in input manager
	std::vector<action_id_t> hints;
};


/**
 * Performs the effect of an action.
 */
using action_func_t = std::function<void(const action_arg_t &)>;


/**
 * For receivers of sets of events a bool is returned
 * to indicate if the event was used.
 */
using action_check_t = std::function<bool(const action_arg_t &)>;


}} // openage::input
