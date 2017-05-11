// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp cimport bool
#include <functional>
// pxd: from libcpp.string cimport string
#include <string>
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

/**
 * maps actions to events.
 */
using binding_map_t = std::unordered_map<action_t, Event>;


/**
 * The input manager manages all input layers (hud, game, ...)
 * and triggers the registered actions depending on the active layer.
 *
 * pxd:
 *
 * cppclass InputManager:
 *     bool set_bind(char* bind_char, string action) except +
 *     string get_bind(string action) except +
 */
class InputManager : public InputHandler {

public:

	InputManager(ActionManager *action_manager);

	/**
	 * Return the string representation of the bind assignated to an action.
	 */
	std::string get_bind(const std::string &action);

	/**
	 * Set the given action to be triggered by the given bind (key/mouse
	 * /wheel). Remove previous assignation. Do nothing if either they
	 * given bind or action is invalid/unknow.
	 */
	bool set_bind(const std::string &bind_str, const std::string action);

	/**
	 * Return the string representation of the key event.
	 */
	std::string key_bind_to_string(const Event &ev);

	/**
	 * Return the string representation of the mouse event.
	 */
	std::string mouse_bind_to_string(const Event &ev);

	/**
	 * Return the key representation of the event.
	 */
	std::string wheel_bind_to_string(const Event &ev);

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
	 * register a hotkey context by pushing it onto the stack.
	 *
	 * this adds the given pointer to the `contexts` list.
	 * that way the context lays on "top".
	 *
	 * if other contexts are registered afterwards,
	 * it wanders down the stack, i.e. looses priority.
	 */
	void push_context(InputContext *context);

	/**
	 * removes any matching registered context from the stack.
	 *
	 * the removal is done by finding the given pointer
	 * in the `contexts` lists, then deleting it in there.
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
	 * Edges enum for use with is_mouse_at_edge
	 */
	enum Edges {
		EDGE_LEFT,
		EDGE_RIGHT,
		EDGE_UP,
		EDGE_DOWN
	};

	/**
	 * Query whether cursor is at edgo of screen
	 *
	 * edge variable is enum Edges
	 *
	 * @return true when the mouse is at the queried screen edge, false else.
	 */
	bool is_mouse_at_edge(Edges edge, int window_size);

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

	/**
	 * Return a string representation of active key bindings
	 * from the given context.
	 */
	std::vector<std::string> active_binds(const std::unordered_map<action_t, action_func_t> &ctx_actions) const;

	/**
	 * Get the action manager attached to this input manager.
	 */
	ActionManager *get_action_manager() const;

private:
	modset_t get_mod() const;

	/**
	 * The action manager to used for keybind action lookups.
	 */
	ActionManager *action_manager;

	/**
	 * The global context. Used as fallback.
	 */
	InputContext global_context;

	/**
	 * Maps actions to events.
	 */
	binding_map_t keys;

	/**
	 * Stack of active input contexts.
	 * The most recent entry is pushed on top of the stack.
	 */
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
	 * mouse position in the window
	 */
	coord::window mouse_position;

	/**
	 * mouse position relative to the last frame position.
	 */
	coord::window_delta mouse_motion;

	friend InputContext;
};

}} // openage::input
