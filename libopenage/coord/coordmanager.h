// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "phys.h"
#include "pixel.h"
#include "tile.h"

namespace openage {
namespace coord {


/**
 * Manager class for all coordinate-related state and metadata.
 * Among other things, this manages the camera positions,
 * and provides about O(n**2) converters between coordinate systems.
 */
class CoordManager final {
public:
	explicit CoordManager() {};

	/**
	 * What's the current size of the window?
	 * (what's the coordinate of the bottom right pixel + (1, 1)?)
	 */
	window_delta window_size{800, 600};

	/**
	 * What place (in Phys3) is the origin of the CamGame coordinate system looking at?
	 */
	phys3 camgame_phys{10, 10, 0};

	/**
	 * Where in the window is the origin of the CamGame coordinate system?
	 * (this is usually the center of the window).
	 */
	window camgame_window{400, 300};

	/**
	 * Where in the window is the origin of the CamHUD coordinate system?
	 * (this is usually the bottom left corner of the window)
	 */
	window camhud_window{0, 600};

	/**
	 * the half size of a tile, in pixels.
	 * tile diamonds are 96 pixels wide and 48 pixels high.
	 * the area of each tile is 96 * 48 * 0.5 square pixels.
	 */
	// TODO: dynamically get from convert script
	camgame_delta tile_halfsize{48, 24};
};

}} // namespace openage::coord
