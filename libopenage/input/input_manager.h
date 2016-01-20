// Copyright 2015-2016 the openage authors. See copying.md for legal info.

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

/**
 * The openage input layer.
 * It gets all the events and processes them accordingly.
 */
namespace input {

using binding_map_t = std::unordered_multimap<Event, action_t, event_hash>;


/**
 * The input manager manages all input layers (hud, game, ...)
 * and triggers the registered actions depending on the active layer.
 */
class InputManager : public openage::InputHandler {

public:
	InputManager();

	/**
	 * returns the global keybind context.
	 * actions bound here will be retained even when override_context is called.
	 */
	InputContext &get_global_context();

	/**
	 * Returns the context on top.
	 * Note there is always a top context
	 * since the global context will be
	 * considered on top when none are registered
	 */
	InputContext &get_top_context();

	/**
	 * register a hotkey context.
	 */
	void register_context(InputContext *context);

	/**
	 * removes any matching registered context.
	 */
	void remove_context(InputContext *context);

	/**
	 * true if the given event type is being ignored
	 */
	bool ignored(const Event &e);

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

	/**
	 * When a SDL event happens, this is called.
	 */
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
	 * mode where mouse position is ignored
	 * used for map scrolling
	 */
	bool relative_mode;

	/**
	 * mouse position and postion delta
	 */
	coord::window mouse_position;
	coord::window_delta mouse_motion;

	friend InputContext;
};

}} // openage::input

#endif
