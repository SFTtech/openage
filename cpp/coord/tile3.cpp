// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#define GEN_IMPL_TILE3_CPP
#include "tile3.h"

#include "tile.h"
#include "phys3.h"

namespace openage {
namespace coord {

tile tile3::to_tile() const {
	return tile {ne, se};
}

tile_delta tile3_delta::to_tile() const {
	return tile_delta {ne, se};
}

phys3 tile3::to_phys3(phys3_delta frac) {
	phys3 result;
	result.ne = phys_t{ne} + frac.ne;
	result.se = phys_t{se} + frac.se;
	result.up = phys_t{up} + frac.up;
	return result;
}

} // namespace coord
} // namespace openage
