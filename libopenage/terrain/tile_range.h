// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "../coord/tile.h"
#include "../coord/phys3.h"

namespace openage {

/**
 * A rectangle or square of tiles which is the minimim
 * space to fit the units foundation or radius
 * the end tile will have ne and se values greater or equal to
 * the start tile
 */
struct tile_range {
	coord::tile start;
	coord::tile end;	// start <= end
	coord::phys3 draw;	// gets used as center point of radial objects
};

}
