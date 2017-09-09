// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <memory>
#include <experimental/optional>
#include <vector>

#include <SDL2/SDL.h>

#include "opengl/context.h"
#include "../coord/window.h"


namespace openage {
namespace renderer {

class Window {
public:
	using key_cb_t = std::function<void(SDL_KeyboardEvent const&)>;
	using mouse_button_cb_t = std::function<void(SDL_MouseButtonEvent const&)>;
	using mouse_wheel_cb_t = std::function<void(SDL_MouseWheelEvent const&)>;
	using resize_cb_t = std::function<void()>;

	/// Create a shiny window with the given title.
	Window(const char *title);
	~Window();

	/// A window cannot be copied.
	Window(const Window &other) = delete;
	Window &operator =(const Window &other) = delete;

	/// But it can be moved.
	Window(Window &&other);
	Window &operator =(Window &&other);

	/// Force the window to the given size. It's generally not a good idea to use this,
	/// as it makes the window jump around wierdly.
	void set_size(const coord::window &new_size);

	/// Returns the dimensions of this window.
	coord::window get_size() const;

	/// Returns true if this window should be closed.
	bool should_close() const;

	/// Polls for window events, calls callbacks for these events, swaps front and back framebuffers
	/// to present graphics onto screen. This has to be called at the end of every graphics frame.
	void update();

	/// Make this window's context the current rendering context of the current thread.
	/// Only use this and most other GL functions on a dedicated window thread.
	void make_context_current();

	// TODO: this shouldn't be exposed.
	// It is only necessary because GuiBasic requires a raw SDL_Window handle, since the QtQuick
	// gui needs the native context of our SDL window to work, so it (the QtQuick gui) is inherently
	// coupled to our window. This probably means that the gui should be integrated either _into_
	// this Window class or _together_ with it in a conceptual unit that manages the GL context.
	/// Returns the raw SDL window handle.
	SDL_Window *get_raw_window() const;

	/// Return a pointer to this window's GL context.
	opengl::GlContext *get_context();

	void add_key_callback(key_cb_t);
	void add_mouse_button_callback(mouse_button_cb_t);
	void add_mouse_wheel_callback(mouse_wheel_cb_t);
	void add_resize_callback(resize_cb_t);

// TODO should be private
protected:
	bool _should_close = false;

	/// The current size of the framebuffer.
	coord::window size;
	/// The SDL struct representing this window.
	SDL_Window *window;

	std::vector<key_cb_t> on_key;
	std::vector<mouse_button_cb_t> on_mouse_button;
	std::vector<mouse_wheel_cb_t> on_mouse_wheel;
	std::vector<resize_cb_t> on_resize;

	/// This window's OpenGL context. It's optional because it can't be constructed immediately,
	/// but after the constructor runs it's guaranteed to be available.
	std::experimental::optional<opengl::GlContext> context;
};

}} // namespace openage::renderer
