// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GAME_RENDERER_H_
#define OPENAGE_GAME_RENDERER_H_

#include <SDL2/SDL.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "engine.h"
#include "coord/tile.h"
#include "handlers.h"
#include "player.h"

namespace openage {

/**
 * renders the editor and action views
 */
class GameRenderer : openage::DrawHandler {
public:
	GameRenderer(openage::Engine *e);
	~GameRenderer();

	bool on_draw() override;

	/**
	 * debug function that draws a simple overlay grid
	 */
	void draw_debug_grid();

	/**
	 * the game this renderer is using
	 */
	GameMain *game() const;

	/**
	 * GameSpec used by this renderer
	 */
	GameSpec *game_spec() const;

	Texture *gaben;

	// TODO shared renderer state,
	// include fog drawing etc
	bool debug_grid_active;

private:
	openage::Engine *engine;

};

} //namespace openage

#endif
