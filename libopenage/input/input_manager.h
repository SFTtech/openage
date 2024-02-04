// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include "coord/pixel.h"
#include "input/action.h"

namespace qtgui {
class GuiInput;
}

namespace openage::input {

namespace camera {
class Controller;
} // namespace camera

namespace game {
class Controller;
} // namespace game

namespace hud {
class Controller;
} // namespace hud

class InputContext;

/**
 * The input manager tracks input signals from peripherals or the
 * GUI and turns them into input events that execute some action.
 */
class InputManager {
public:
	InputManager();

	~InputManager() = default;

	/**
	 * Set the GUI input handler.
	 *
	 * @param gui_input GUI input handler.
	 */
	void set_gui(const std::shared_ptr<qtgui::GuiInput> &gui_input);

	/**
	 * Set the controller for the camera.
	 *
	 * @param controller Camera controller.
	 */
	void set_camera_controller(const std::shared_ptr<camera::Controller> &controller);

	/**
	 * Set the controller for the game simulation.
	 *
	 * @param controller Game controller.
	 */
	void set_game_controller(const std::shared_ptr<game::Controller> &controller);

	/**
	 * Set the controller for the HUD.
	 *
	 * @param controller HUD controller.
	 */
	void set_hud_controller(const std::shared_ptr<hud::Controller> controller);

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
	 * Get the info for all events which are bound currently.
	 */
	std::vector<std::string> active_binds() const;

	/**
	 * Push a context on top of the stack, making it the
	 * current top context.
	 *
	 * if other contexts are registered afterwards,
	 * it wanders down the stack, i.e. loses priority.
	 */
	void push_context(const std::shared_ptr<InputContext> &context);

	/**
	 * Push the context with the specified ID on top of the stack,
	 * making it the current top context.
	 *
	 * if other contexts are registered afterwards,
	 * it wanders down the stack, i.e. loses priority.
	 */
	void push_context(const std::string &id);

	/**
	 * Remove the current top context from the stack.
	 */
	void pop_context();

	/**
	 * Removes any registered context matching the specified ID from the stack.
	 *
	 * the removal is done by finding the given pointer
	 * in the `active_contexts` lists, then deleting it in there.
	 */
	void pop_context(const std::string &id);

	/**
	 * Remove a context from the available contexts.
	 */
	void remove_context(const std::string &id);

	/**
	 * Add a context to the available contexts.
	 */
	void add_context(const std::shared_ptr<InputContext> context);

	/**
	 * updates mouse position state and motion
	 */
	void set_mouse(int x, int y);

	/**
	 * updates mouse motion only
	 */
	void set_motion(int x, int y);

	/**
	 * Process an input event from the Qt window management.
	 *
	 * @param ev Qt input event.
	 *
	 * @return true if the event is accepted, else false.
	 */
	bool process(const QEvent &ev);


private:
	/**
	 * Process the (default) action for an input event.
	 *
	 * @param ev Input event.
	 * @param action Action bound to the event.
	 * @param bind_ctx Context the action is bound in.
	 */
	void process_action(const input::Event &ev,
	                    const input_action &action,
	                    const std::shared_ptr<InputContext> &ctx);

	/**
	 * The global context. Used as fallback.
	 */
	std::shared_ptr<InputContext> global_context;

	/**
	 * Stack of active input contexts.
	 * The most recent entry is pushed on top of the stack.
	 */
	std::vector<std::shared_ptr<InputContext>> active_contexts;

	/**
	 * Map of all available contexts, referencable by an ID.
	 *
	 * TODO: Move this to cvar manager?
	 */
	std::unordered_map<std::string, std::shared_ptr<InputContext>> available_contexts;

	/**
	 * Interface to the game simulation.
	 */
	std::shared_ptr<game::Controller> game_controller;

	/**
	 * Interface to the camera.
	 */
	std::shared_ptr<camera::Controller> camera_controller;

	/**
	 * Interface to the HUD.
	 */
	std::shared_ptr<hud::Controller> hud_controller;

	/**
	 * Interface to the GUI.
	 */
	std::shared_ptr<qtgui::GuiInput> gui_input;

	/**
	 * mouse position in the window
	 */
	coord::input mouse_position{0, 0};

	/**
	 * mouse position relative to the last frame position.
	 */
	coord::input_delta mouse_motion{0, 0};
};

/**
 * Setup default input actions:
 *
 * - Camera movement
 *
 * TODO: Make this configurable.
 *
 * @param ctx Input context the actions are added to.
 */
void setup_defaults(const std::shared_ptr<InputContext> &ctx);

} // namespace openage::input
