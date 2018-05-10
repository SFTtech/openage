// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <SDL2/SDL.h>

#include "../coord/window.h"
#include "renderer.h"


namespace openage {
namespace renderer {

class Window {
public:
	virtual ~Window() = default;

	/// Returns the dimensions of this window.
	coord::window get_size() const;

	/// Returns true if this window should be closed.
	bool should_close() const;

	using key_cb_t = std::function<void(SDL_KeyboardEvent const&)>;
	using mouse_button_cb_t = std::function<void(SDL_MouseButtonEvent const&)>;
	using mouse_wheel_cb_t = std::function<void(SDL_MouseWheelEvent const&)>;
	using resize_cb_t = std::function<void(coord::window)>;

	void add_key_callback(key_cb_t);
	void add_mouse_button_callback(mouse_button_cb_t);
	void add_mouse_wheel_callback(mouse_wheel_cb_t);
	void add_resize_callback(resize_cb_t);

	/// Force the window to the given size. It's generally not a good idea to use this,
	/// as it makes the window jump around wierdly.
	virtual void set_size(const coord::window &new_size) = 0;

	/// Polls for window events, calls callbacks for these events, swaps front and back framebuffers
	/// to present graphics onto screen. This has to be called at the end of every graphics frame.
	virtual void update() = 0;

	/// Creates a renderer which uses the window's graphics API and targets the window.
	virtual std::shared_ptr<Renderer> make_renderer() = 0;

protected:
	Window(coord::window size);

	bool should_be_closed = false;

	/// The current size of the framebuffer.
	coord::window size;

	std::vector<key_cb_t> on_key;
	std::vector<mouse_button_cb_t> on_mouse_button;
	std::vector<mouse_wheel_cb_t> on_mouse_wheel;
	std::vector<resize_cb_t> on_resize;
};

}} // namespace openage::renderer
