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

	coord::window get_size();
	void set_size(const coord::window &, bool update=false);

	Window(const char *title);
	~Window();

	void swap();

private:
	coord::window size;
	SDL_Window *window;

	std::unique_ptr<Context> context;
};

}} // namespace openage::renderer

#endif
