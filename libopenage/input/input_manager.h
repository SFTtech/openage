// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_INPUT_INPUT_MANAGER_H_
#define OPENAGE_INPUT_INPUT_MANAGER_H_

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
	 * register a hotkey context.
	 */
	void register_context(InputContext *context);

	/**
	 * removes any matching registered context.
	 */
	void remove_context(InputContext *context);

	/**
	 * manages the pressing of a key.
	 * first checks whether an action is bound to it.
	 * if it is, look for an handler to execute that handler.
	 * returns true if the event was responded to
	 */
	bool trigger(const Event &e);

	/**
	 * sets the state of a specific key
	 */
	void set_state(const Event &ev, bool is_down);

	/**
	 * updates mouse position state and motion
	 */
	void set_mouse(int x, int y);

	/**
	 * updates mouse motion only
	 */
	void set_motion(int x, int y);

	/**
	 * enable relative mouse mode
	 */
	void set_relative(bool mode);

	/**
	 * Query stored pressing stat for a key.
	 *
	 * note that the function stores a unknown/new keycode
	 * as 'not pressed' if requested
	 * @return true when the key is pressed, false else.
	 */
	bool is_down(const ClassCode &cc) const;
	bool is_down(event_class ec, code_t code) const;

	/**
	 * Most cases should use above is_down(class, code)
	 * instead to avoid relying on sdl types
	 *
	 * Query stored pressing stat for a key.
	 * @return true when the key is pressed, false else.
	 */
	bool is_down(SDL_Keycode k) const;

	/**
	 * Checks whether a key modifier is held down.
	 */
	bool is_mod_down(modifier mod) const;


	bool on_input(SDL_Event *e) override;

private:
	modset_t get_mod() const;

	InputContext global_hotkeys;
	binding_map_t keys;
	std::vector<InputContext *> contexts;

	/**
	 * key to is_down map.
	 * stores a mapping between keycodes and its pressing state.
	 * a true value means the key is currently pressed,
	 * false indicates the key is untouched.
	 */
	std::unordered_map<ClassCode, bool, class_code_hash> states;

	/**
	 * Current key modifiers.
	 * Included ALL modifiers including num lock and caps lock.
	 */
	modset_t keymod;

	/**
	 * current mouse position
	 */
	bool relative_mode;
	coord::window mouse_position;
	coord::window_delta mouse_motion;

};

} //namespace input
} //namespace openage

#endif
