// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#pragma once

#include "phys.h"
#include "pixel.h"
#include "tile.h"

namespace openage {
namespace coord {


/**
 * Holds all coordinate-related state and metadata.
 *
 * Among other things, this stores the camera positions.
 */
class CoordManager final {
public:
	explicit CoordManager() {};

	/**
	 * What's the current size of the viewport?
	 * (what's the coordinate of the top right pixel + (1, 1)?)
	 */
	viewport_delta viewport_size{800, 600};

	/**
	 * What place (in Phys3) is the origin of the CamGame coordinate system looking at?
	 */
	phys3 camgame_phys{10, 10, 0};

	/**
	 * Where in the viewport is the origin of the CamGame coordinate system?
	 * (this is usually the center of the viewport).
	 */
	viewport camgame_viewport{400, 300};

	/**
	 * Where in the viewport is the origin of the CamHUD coordinate system?
	 * (this is usually the bottom left corner of the viewport)
	 */
	viewport camhud_viewport{0, 0};

	/**
	 * The size of a terrain tile, in pixels.
	 * Rile diamonds are 96 pixels wide and 48 pixels high.
	 * The area of each tile is 96 * 48 * 0.5 square pixels.
	 */
	// \todo dynamically get from nyan data
	camgame_delta tile_size{96, 48};
};

}} // namespace openage::coord
