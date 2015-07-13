// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_KEYBINDS_KEYBIND_MANAGER_H_
#define OPENAGE_KEYBINDS_KEYBIND_MANAGER_H_

#include <functional>
#include <map>
#include <unordered_map>
#include <SDL2/SDL.h>

#include "../handlers.h"
#include "action.h"
#include "event.h"
#include "input_context.h"

namespace openage {
namespace input {

using binding_map_t = std::unordered_multimap<Event, action_t, event_hash>;

class InputManager : public openage::InputHandler {

public:
	InputManager();

	/**
	 * returns the global keybind context.
	 * actions bound here will be retained even when override_context is called.
	 */
	InputContext &get_global_context();

	/**
	 * register a hotkey context, and override old keybinds.
	 */
	void override_context(InputContext *context);

	/**
	 * register a hotkey context.
	 */
	void register_context(InputContext *context);

	/**
	 * removes the most recently registered context.
	 */
	void remove_context();

	/**
	 * manages the pressing of a key.
	 * first checks whether an action is bound to it.
	 * if it is, look for an handler to execute that handler.
	 */
	void trigger(const Event &e);

	/**
	 * sets the state of a specific key
	 */
	void set_state(const Event &ev, bool is_down);

	/**
	 * query stored pressing stat for a key.
	 * @return true when the key is pressed, false else.
	 */
	bool is_down(SDL_Keycode k);

	/**
	 * Checks whether a key modifier is held down.
	 */
	bool is_mod_down(SDL_Keymod mod) const;


	bool on_input(SDL_Event *e) override;

private:
	modset_t get_mod() const;

	InputContext global_hotkeys;
	binding_map_t keys;
	std::stack<std::vector<InputContext *>> contexts;

	/**
	 * key to is_down map.
	 * stores a mapping between keycodes and its pressing state.
	 * a true value means the key is currently pressed,
	 * false indicates the key is untouched.
	 */
	std::unordered_map<class_code_t, bool, class_code_hash> states;

	/**
	 * Current key modifiers.
	 * Included ALL modifiers including num lock and caps lock.
	 */
	modset_t keymod;

	/**
	 * current mouse position
	 */
	coord::window mouse_position;

	/**
	 * used key modifiers to check for.
	 * this excludes keys like num lock and caps lock, which would
	 * otherwise mess up key combinations
	 */
	static constexpr SDL_Keymod used_keymods = static_cast<SDL_Keymod>(KMOD_CTRL | KMOD_SHIFT | KMOD_ALT | KMOD_GUI);
};

} //namespace input
} //namespace openage

#endif
