// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <functional>
#include <utility>
#include <experimental/optional>

#include <SDL2/SDL.h>

#include "opengl/context_provider.h"
#include "opengl/context.h"
#include "../coord/window.h"


namespace openage {
namespace renderer {

class Window {
public:
	Window(const Window &other) = delete;
	Window(Window &&other) = delete;
	Window &operator =(const Window &other) = delete;
	Window &operator =(Window &&other) = delete;

	/**
	 * Create a shiny window with the given title.
	 */
	Window(const char *title);
	~Window();

	/**
	 * @returns the window dimensions
	 */
	coord::window get_size();

	/**
	 * Resize the window's client area.
	 * @param[in] update determines what?
	 */
	void set_size(const coord::window &new_size);

	/**
	 * Swaps the back and front framebuffers.
	 * Used to actually display the newly rendered frame on the screen.
	 */
	void swap();

	/// Make this context the current rendering context of the window thread.
	/// Only use this and most other GL functions on the window thread.
	void make_context_current();

	// TODO: this shouldn't be exposed.
	// It is only necessary because GuiBasic requires a raw SDL_Window handle, since the QtQuick
	// gui needs the native context of our SDL window to work, so it (the QtQuick gui) is inherently
	// coupled to our window. This probably means that the gui should be integrated either _into_
	// this Window class or _together_ with it in a conceptual unit that manages the GL context.
	/**
	 * Return the raw SDL window handle.
	 */
	SDL_Window* get_raw_window() const;

	/**
	 * Return a reference to this window's GL context.
	 */
	opengl::GlContext* get_context();

private:
	coord::window size;
	SDL_Window *window;

	std::experimental::optional<opengl::GlContext> context;
};

}} // namespace openage::renderer
