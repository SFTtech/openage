// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <SDL2/SDL.h>

#include "coord/window.h"


namespace openage {

/**
 * superclass for all possible drawing operations in the game.
 */
class DrawHandler {
public:
	/**
	 * execute the drawing action.
	 */
	virtual bool on_draw() = 0;
};

/**
 * superclass for all possible drawing operations in the game.
 */
class HudHandler {
public:
	/**
	 * execute the drawing action.
	 */
	virtual bool on_drawhud() = 0;
};

/**
 * superclass for all calculations being done on engine tick.
 */
class TickHandler {
public:
	/**
	 * execute the tick action.
	 */
	virtual bool on_tick() = 0;
};

/**
 * superclass for handling any input event.
 */
class InputHandler {
public:
	/**
	 * execute the input handler.
	 */
	virtual bool on_input(SDL_Event *event) = 0;
};

/**
 * superclass for handling a window resize event.
 */
class ResizeHandler {
public:
	/**
	 * execute the resize handler.
	 */
	virtual bool on_resize(coord::window new_size) = 0;
};

}
