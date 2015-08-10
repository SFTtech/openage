// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_WINDOW_H_
#define OPENAGE_RENDERER_WINDOW_H_

#include "context.h"

#include <memory>
#include <SDL2/SDL.h>

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
	 * Resize the drawing window.
	 */
	void set_size(const coord::window &new_size, bool update=false);

	/**
	 * Swaps the back and front framebuffers.
	 * Used to actually display the newly rendered frame.
	 */
	void swap();

	/**
	 * Return the context created for this window.
	 */
	std::shared_ptr<Context> get_context();


private:
	coord::window size;
	SDL_Window *window;

	std::shared_ptr<Context> context;
};

}} // namespace openage::renderer

#endif
