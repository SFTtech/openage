// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GAME_RENDERER_H_
#define OPENAGE_GAME_RENDERER_H_

#include <SDL2/SDL.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "coord/tile.h"
#include "engine.h"
#include "game_main.h"
#include "handlers.h"
#include "options.h"
#include "player.h"

namespace openage {

// TODO include fog drawing etc
struct render_settings {
public:
	render_settings();

	bool draw_grid;
	bool draw_debug;
	bool terrain_blending;
};

/**
 * renders the editor and action views
 *
 */
class GameRenderer : DrawHandler, options::OptionNode {
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

	render_settings settings;

private:
	openage::Engine *engine;

};

} //namespace openage

#endif
