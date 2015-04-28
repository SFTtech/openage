// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_KEYBINDS_KEYBIND_CONTEXT_H_
#define OPENAGE_KEYBINDS_KEYBIND_CONTEXT_H_

#include <functional>
#include <stack>
#include <unordered_map>
#include <vector>

#include "action.h"
#include "../handlers.h"

namespace openage {
namespace keybinds {

class KeybindContext {

public:
	KeybindContext();

	/**
	 * bind a function to an action.
	 */
	void bind(const action_t a, const std::function<void()> f);

	/**
	 * lookup an action. If it is bound, execute it.
	 * @return true when the action is executed, false else.
	 */
	bool execute_if_bound(action_t a);
private:
	/**
	 * action to function map
	 */
	std::unordered_map<action_t, std::function<void()>, action_hash> binds;
};

} //namespace keybinds
} //namespace openage

#endif
