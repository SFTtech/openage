// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <QObject>

#include "renderer/renderer.h"
#include "renderer/types.h"
#include "util/vector.h"

QT_FORWARD_DECLARE_CLASS(QWindow)
QT_FORWARD_DECLARE_CLASS(QKeyEvent)
QT_FORWARD_DECLARE_CLASS(QMouseEvent)
QT_FORWARD_DECLARE_CLASS(QWheelEvent)

namespace openage::renderer {

class WindowEventHandler;

/**
 * Modes for window display.
 */
enum class window_mode {
	FULLSCREEN,
	BORDERLESS,
	WINDOWED
};

/**
 * Settings for creating a window.
 */
struct window_settings {
	// Width of the window in pixels.
	size_t width = 1024;
	// Height of the window in pixels.
	size_t height = 768;
	// Graphics API to use in the window's renderer.
	graphics_api_t backend = graphics_api_t::DEFAULT;
	// If true, enable vsync.
	bool vsync = true;
	// If true, enable debug logging for the selected backend.
	bool debug = false;
	// Display mode for the window.
	window_mode mode = window_mode::WINDOWED;
};


/**
 * Represents a window that can be used to display graphics.
 */
class Window {
public:
	/**
	 * Create a new Window instance for displaying stuff.
	 *
	 * @param title Window title shown in the Desktop Environment.
	 * @param settings Settings for creating the window.
	 *
	 * @return The created Window instance.
	 */
	static std::shared_ptr<Window> create(const std::string &title,
	                                      window_settings settings = {});

	virtual ~Window() = default;

	/**
	 * Get the dimensions of this window.
	 *
	 * @return (width, height) as a size-2 vector.
	 */
	const util::Vector2s &get_size() const;

	/**
	 * Get the scaling factor of the window.
	 *
	 * @return Scaling factor.
	 */
	double get_scale() const;

	/**
	 * Returns \p true if this window should be closed.
	 *
	 * @return true if the window should close, else false.
	 */
	bool should_close() const;

	using key_cb_t = std::function<void(const QKeyEvent &)>;
	using mouse_button_cb_t = std::function<void(const QMouseEvent &)>;
	using mouse_move_cb_t = std::function<void(const QMouseEvent &)>;
	using mouse_wheel_cb_t = std::function<void(const QWheelEvent &)>;
	using resize_cb_t = std::function<void(size_t, size_t, double)>;

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
	 * Register a function that executes when the mouse is moved.
	 *
	 * @param cb Callback function.
	 */
	void add_mouse_move_callback(const mouse_move_cb_t &cb);

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
	 * Current size of the window (in pixels).
	 */
	util::Vector2s size;

	/**
	 * Scaling factor for the window size (also known as "device pixel ratio"
	 * in Qt). Used if OS-level high DPI/fractional scaling is applied.
	 */
	double scale_dpr = 1.0;

	/**
	 * Callbacks for key presses.
	 */
	std::vector<key_cb_t> on_key;

	/**
	 * Callbacks for mouse button presses.
	 */
	std::vector<mouse_button_cb_t> on_mouse_button;

	/**
	 * Callbacks for mouse move actions.
	 */
	std::vector<mouse_move_cb_t> on_mouse_move;

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
