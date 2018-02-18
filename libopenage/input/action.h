// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

#include "../coord/pixel.h"
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


// TODO this whole class seems rather obsolete...
//      remove it and instead provide a proper class for action_t?
/**
 * The action manager manages all the actions allow creation, access
 * information and equality.
 */
class ActionManager {
public:
	ActionManager(InputManager *input_manager,
	              cvar::CVarManager *cvar_manager);

public:
	action_t get(const std::string &type);
	std::string get_name(const action_t action);
	bool is(const std::string &type, const action_t action);

private:
	bool create(const std::string& type);

	// mapping from action name to numbers
	std::unordered_map<std::string, action_t> actions;
	// for high-speed reverse lookups (action number -> name)
	std::unordered_map<action_t, std::string> reverse_map;

	InputManager *const input_manager;
	cvar::CVarManager *const cvar_manager;

	// the id of the next action that is added via create().
	action_t next_action_id = 0;
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
	const coord::input mouse;
	const coord::input_delta motion;

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
