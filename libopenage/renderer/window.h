// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <SDL2/SDL.h>

#include "../util/vector.h"
#include "renderer.h"

#include <QObject>

QT_FORWARD_DECLARE_CLASS(QWindow)

namespace openage::renderer {

class WindowEventHandler;

class Window {
public:
	/**
	 * Create a new Window instance for displaying stuff.
	 *
	 * @param title Window title shown in the Desktop Environment.
	 * @param width Width in pixels.
	 * @param height Height in pixels.
	 * @return The created Window instance.
	 */
	static std::shared_ptr<Window> create(const std::string &title,
	                                      size_t width,
	                                      size_t height);

	virtual ~Window() = default;

	/**
	 * Get the dimensions of this window.
	 *
	 * @return (width, height) as a size-2 vector.
	 */
	const util::Vector2s &get_size() const;

	/**
	 * Returns \p true if this window should be closed.
	 *
	 * @return true if the window should close, else false.
	 */
	bool should_close() const;

	using key_cb_t = std::function<void(SDL_KeyboardEvent const &)>;
	using mouse_button_cb_t = std::function<void(SDL_MouseButtonEvent const &)>;
	using mouse_wheel_cb_t = std::function<void(SDL_MouseWheelEvent const &)>;
	using resize_cb_t = std::function<void(size_t, size_t)>;

	/**
	 * Register a function that executes when a key is pressed.
	 *
	 * @param cb Callback function.
	 */
	void add_key_callback(const key_cb_t &cb);

	/**
	 * Register a function that executes when a mouse button is pressed.
	 *
	 * @param cb Callback function.
	 */
	void add_mouse_button_callback(const mouse_button_cb_t &cb);

	/**
	 * Register a function that executes when a mouse wheel action is used.
	 *
	 * @param cb Callback function.
	 */
	void add_mouse_wheel_callback(const mouse_wheel_cb_t &cb);

	/**
	 * Register a function that executes when the window is resized.
	 *
	 * @param cb Callback function.
	 */
	void add_resize_callback(const resize_cb_t &cb);

	/**
	 * Get the underlying QWindow that is used for drawing.
	 *
	 * @return Pointer to the QWindow.
	 */
	const std::shared_ptr<QWindow> &get_qt_window() const;

	/**
	 * Force this window to the given size. It's generally not a good idea to use this,
	 * as it makes the window jump around wierdly.
	 *
	 * @param width Width in pixels.
	 * @param height Height in pixels.
	 */
	virtual void set_size(size_t width, size_t height) = 0;

	/**
	 * Polls for window events, calls callbacks for these events, swaps front and back framebuffers
	 * to present graphics onto screen. This has to be called at the end of every graphics frame.
	 */
	virtual void update() = 0;

	/**
	 * Creates a renderer which uses the window's graphics API and targets the window.
	 *
	 * @return The created Renderer instance.
	 */
	virtual std::shared_ptr<Renderer> make_renderer() = 0;

	/**
	 * Instruct the Window widget to disappear.
	 */
	void close();

protected:
	Window(size_t width, size_t height);

	/**
	 * Determines if the window should be closed.
	 */
	bool should_be_closed = false;

	/**
	 * Current size of the framebuffer.
	 */
	util::Vector2s size;

	/**
	 * Callbacks for key presses.
	 */
	std::vector<key_cb_t> on_key;

	/**
	 * Callbacks for mouse button presses.
	 */
	std::vector<mouse_button_cb_t> on_mouse_button;

	/**
	 * Callbacks for mouse wheel actions.
	 */
	std::vector<mouse_wheel_cb_t> on_mouse_wheel;

	/**
	 * Callbacks for resize actions.
	 */
	std::vector<resize_cb_t> on_resize;

	/**
	 * Main Qt window handle.
	 */
	std::shared_ptr<QWindow> window;

	/**
	 * Observes and filters events from the Qt window.
	 * Gets attached to window in the window subclasses.
	 */
	std::shared_ptr<WindowEventHandler> event_handler;
};

} // namespace openage::renderer
