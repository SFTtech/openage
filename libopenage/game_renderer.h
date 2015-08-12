// Copyright 2015-2015 the openage authors. See copying.md for legal info.

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

#include "minimap.h"

namespace openage {
/* namespace minimap_shader { */
/* extern shader::Program *program; */
/* extern GLint size, orig, color; */
/* } */


/**
 * Options for the renderer.
 * These will be included in the user interface
 * via reflection, so adding new members will
 * always be visible
 *
 * TODO include fog drawing etc
 */
class RenderOptions : public options::OptionNode {
public:
	RenderOptions();

	options::Var<bool> draw_grid;
	options::Var<bool> draw_debug;
	options::Var<bool> terrain_blending;
};

/**
 * renders the editor and action views
 *
 */
class GameRenderer : DrawHandler {
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
  Minimap *minimap;

	RenderOptions settings;

private:
	openage::Engine *engine;

};

} // openage

#endif
