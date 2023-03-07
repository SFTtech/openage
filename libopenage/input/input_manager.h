// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

#include "coord/pixel.h"

namespace openage::input {

class InputContext;
class Controller;

/**
 * The input manager tracks input signals from peripherals or the
 * GUI and turns them into input events that execute some action.
 */
class InputManager {
public:
	InputManager();

	/**
	 * returns the global keybind context.
	 * actions bound here will be retained even when override_context is called.
	 */
	const std::shared_ptr<InputContext> &get_global_context();

	/**
	 * Returns the context on top.
	 * Note there is always a top context
	 * since the global context will be
	 * considered on top when none are registered
	 */
	const std::shared_ptr<InputContext> &get_top_context();

	/**
	 * a list of all keys of the active context
	 * which are bound currently.
	 */
	std::vector<std::string> active_binds() const;

	/**
	 * register a hotkey context by pushing it onto the stack.
	 *
	 * this adds the given pointer to the `contexts` list.
	 * that way the context lays on "top".
	 *
	 * if other contexts are registered afterwards,
	 * it wanders down the stack, i.e. looses priority.
	 */
	void push_context(const std::shared_ptr<InputContext> &context);

	/**
	 * removes any matching registered context from the stack.
	 *
	 * the removal is done by finding the given pointer
	 * in the `contexts` lists, then deleting it in there.
	 */
	void remove_context(const std::shared_ptr<InputContext> &context);

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

private:
	/**
	 * The global context. Used as fallback.
	 */
	std::shared_ptr<InputContext> global_context;

	/**
	 * Stack of active input contexts.
	 * The most recent entry is pushed on top of the stack.
	 */
	std::vector<std::shared_ptr<InputContext>> contexts;

	/**
     * Interface to the engine.
     */
	std::shared_ptr<Controller> controller;

	/**
	 * mouse position in the window
	 */
	coord::input mouse_position{0, 0};

	/**
	 * mouse position relative to the last frame position.
	 */
	coord::input_delta mouse_motion{0, 0};
};

} // namespace openage::input
