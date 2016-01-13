// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_INPUT_ACTION_H_
#define OPENAGE_INPUT_ACTION_H_

#include <vector>

#include "../coord/window.h"
#include "event.h"

namespace openage {
namespace input {

struct action_t {
	int key;
	const char *info;

	bool operator ==(const action_t &other) const;
};

/**
 * All available input actions.
 */
namespace actions {
static constexpr action_t UNDEFINED = action_t{0, "UNDEFINED"};
static constexpr action_t START_GAME = action_t{1, "START_GAME"};
static constexpr action_t STOP_GAME = action_t{2, "STOP_GAME"};
static constexpr action_t TOGGLE_HUD = action_t{3, "TOGGLE_HUD"};
static constexpr action_t SCREENSHOT = action_t{4, "SCREENSHOT"};
static constexpr action_t TOGGLE_DEBUG_OVERLAY = action_t{5, "TOGGLE_DEBUG_OVERLAY"};
static constexpr action_t TOGGLE_DEBUG_GRID = action_t{6, "TOGGLE_DEBUG_GRID"};
static constexpr action_t QUICK_SAVE = action_t{7, "QUICK_SAVE"};
static constexpr action_t QUICK_LOAD = action_t{8, "QUICK_LOAD"};
static constexpr action_t TOGGLE_MODE = action_t{9, "TOGGLE_MODE"};
static constexpr action_t TOGGLE_MENU = action_t{10, "TOGGLE_MENU"};
static constexpr action_t TOGGLE_ITEM = action_t{11, "TOGGLE_ITEM"};
static constexpr action_t TOGGLE_BLENDING = action_t{12, "TOGGLE_BLENDING"};
static constexpr action_t TOGGLE_PROFILER = action_t{13, "TOGGLE_PROFILER"};
static constexpr action_t TOGGLE_CONSTRUCT_MODE = action_t{14, "TOGGLE_CONSTRUCT_MODE"};
static constexpr action_t TOGGLE_UNIT_DEBUG = action_t{15, "TOGGLE_UNIT_DEBUG"};
static constexpr action_t TRAIN_OBJECT = action_t{16, "TRAIN_OBJECT"};
static constexpr action_t ENABLE_BUILDING_PLACEMENT = action_t{17, "ENABLE_BUILDING_PLACEMENT"};
static constexpr action_t DISABLE_SET_ABILITY = action_t{18, "DISABLE_SET_ABILITY"};
static constexpr action_t SET_ABILITY_MOVE = action_t{19, "SET_ABILITY_MOVE"};
static constexpr action_t SET_ABILITY_GATHER = action_t{20, "SET_ABILITY_GATHER"};
static constexpr action_t SET_ABILITY_GARRISON = action_t{21, "SET_ABILITY_GARRISON"};
static constexpr action_t TOGGLE_CONSOLE = action_t{22, "TOGGLE_CONSOLE"};
static constexpr action_t SPAWN_VILLAGER = action_t{23, "SPAWN_VILLAGER"};
static constexpr action_t KILL_UNIT = action_t{24, "KILL_UNIT"};
static constexpr action_t BUILDING_1 = action_t{25, "BUILDING_1"};
static constexpr action_t BUILDING_2 = action_t{26, "BUILDING_2"};
static constexpr action_t BUILDING_3 = action_t{27, "BUILDING_3"};
static constexpr action_t BUILDING_4 = action_t{28, "BUILDING_4"};
static constexpr action_t BUILDING_TOWN_CENTER = action_t{29, "BUILDING_TOWN_CENTER"};
static constexpr action_t SWITCH_TO_PLAYER_1 = action_t{30, "SWITCH_TO_PLAYER_1"};
static constexpr action_t SWITCH_TO_PLAYER_2 = action_t{31, "SWITCH_TO_PLAYER_2"};
static constexpr action_t SWITCH_TO_PLAYER_3 = action_t{32, "SWITCH_TO_PLAYER_3"};
static constexpr action_t SWITCH_TO_PLAYER_4 = action_t{33, "SWITCH_TO_PLAYER_4"};
static constexpr action_t SWITCH_TO_PLAYER_5 = action_t{34, "SWITCH_TO_PLAYER_5"};
static constexpr action_t SWITCH_TO_PLAYER_6 = action_t{35, "SWITCH_TO_PLAYER_6"};
static constexpr action_t SWITCH_TO_PLAYER_7 = action_t{36, "SWITCH_TO_PLAYER_7"};
static constexpr action_t SWITCH_TO_PLAYER_8 = action_t{37, "SWITCH_TO_PLAYER_8"};
static constexpr action_t UP_ARROW = action_t{38, "UP_ARROW"};
static constexpr action_t DOWN_ARROW = action_t{39, "DOWN_ARROW"};
static constexpr action_t LEFT_ARROW = action_t{40, "LEFT_ARROW"};
static constexpr action_t RIGHT_ARROW = action_t{41, "RIGHT_ARROW"};
static constexpr action_t SELECT = action_t{42, "SELECT"};
static constexpr action_t DESELECT = action_t{43, "DESELECT"};
static constexpr action_t FORWARD = action_t{44, "FORWARD"};
static constexpr action_t BACK = action_t{45, "BACK"};
static constexpr action_t PAINT_TERRAIN = action_t{46, "PAINT_TERRAIN"};
} // actions

struct action_hash {
	int operator ()(const action_t &a) const;
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

	// hints for arg reciever
	// these get set globally in input manager
	std::vector<action_id_t> hints;
};

/**
 * complete the effect of an action
 */
using action_func_t = std::function<void(const action_arg_t &)>;

/**
 * for recievers of sets of events a bool is returned
 * to indicate if the event was used
 */
using action_check_t = std::function<bool(const action_arg_t &)>;


}} // openage::input

#endif
