// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_KEYBINDS_KEYBIND_MANAGER_H_
#define OPENAGE_KEYBINDS_KEYBIND_MANAGER_H_

#include <functional>
#include <map>
#include <unordered_map>
#include <SDL2/SDL.h>

#include "action.h"
#include "keybind_context.h"
#include "key.h"

namespace openage {
namespace keybinds {

class KeybindManager {

public:
	KeybindManager();

	/**
	 * returns the global keybind context.
	 * actions bound here will be retained even when override_context is called.
	 */
	KeybindContext &get_global_keybind_context();

	/**
	 * register a hotkey context, and override old keybinds.
	 */
	void override_context(KeybindContext *context);

	/**
	 * register a hotkey context.
	 */
	void register_context(KeybindContext *context);

	/**
	 * removes the most recently registered context.
	 */
	void remove_context();

	/**
	 * manages the pressing of a key.
	 * first checks whether an action is bound to it.
	 * if it is, look for an handler to execute that handler.
	 */
	void press(key_t k);

	/**
	 * sets the state of a specific key
	 */
	void set_key_state(SDL_Keycode k, bool is_down);

	/**
	 * query stored pressing stat for a key.
	 * @return true when the key is pressed, false else.
	 */
	bool is_key_down(SDL_Keycode k);
private:
	KeybindContext global_hotkeys;
	std::unordered_map<key_t, action_t, key_hash> keys;
	std::stack<std::vector<KeybindContext *>> contexts;

	/**
	 * key to is_down map.
	 * stores a mapping between keycodes and its pressing state.
	 * a true value means the key is currently pressed,
	 * false indicates the key is untouched.
	 */
	std::unordered_map<SDL_Keycode, bool> key_states;
};

} //namespace keybinds
} //namespace openage

#endif
