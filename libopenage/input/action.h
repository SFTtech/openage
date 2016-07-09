// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_map>
#include <vector>
#include <functional>

#include "../coord/window.h"
#include "event.h"

namespace openage {

class Engine;

namespace input {

using action_t = unsigned int;

using action_map_t = std::unordered_map<std::string, action_t>;

/**
 * The action manager manages all the actions allow creation, access
 * information and equality.
 */
class ActionManager {
public:
	ActionManager(Engine *engine);
	bool create(const std::string type);
	action_t get(const std::string &type) const;
	std::string get_name(const action_t action) const;
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
		"BUILDING_1",
		"BUILDING_2",
		"BUILDING_3",
		"BUILDING_4",
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
	Engine *engine;
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
 * complete the effect of an action
 */
using action_func_t = std::function<void(const action_arg_t &)>;

/**
 * for receivers of sets of events a bool is returned
 * to indicate if the event was used
 */
using action_check_t = std::function<bool(const action_arg_t &)>;


}} // openage::input
