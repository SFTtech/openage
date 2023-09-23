// Copyright 2014-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <SDL2/SDL.h>

#include "coord/pixel.h"


namespace openage {

class LegacyEngine;

/**
 * superclass for all possible drawing operations in the game.
 */
class [[deprecated]] HudHandler {
public:
	virtual ~HudHandler() = default;

	/**
	 * execute the drawing action.
	 */
	virtual bool on_drawhud() = 0;
};

/**
 * superclass for all calculations being done on engine tick.
 */
class [[deprecated]] TickHandler {
public:
	virtual ~TickHandler() = default;

	/**
	 * execute the tick action.
	 */
	virtual bool on_tick() = 0;
};

/**
 * superclass for handling any input event.
 */
class [[deprecated]] InputHandler {
public:
	virtual ~InputHandler() = default;

	/**
	 * execute the input handler.
	 */
	virtual bool on_input(SDL_Event *event) = 0;
};

/**
 * superclass for handling a window resize event.
 */
class [[deprecated]] ResizeHandler {
public:
	virtual ~ResizeHandler() = default;

	/**
	 * execute the resize handler.
	 */
	virtual bool on_resize(coord::viewport_delta new_size) = 0;
};

} // namespace openage
