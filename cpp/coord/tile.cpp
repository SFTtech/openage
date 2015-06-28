// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#define GEN_IMPL_TILE_CPP
#include "tile.h"

#include "tile3.h"
#include "chunk.h"
#include "phys2.h"

namespace openage {
namespace coord {

tile3 tile::to_tile3(tile_t up) const {
	return tile3 {ne, se, up};
}

tile3_delta tile_delta::to_tile3(tile_t up) const {
	return tile3_delta {ne, se, up};
}

phys2 tile::to_phys2(phys2_delta frac) const {
	phys2 result;
	result.ne = phys_t{ne} + frac.ne;
	result.se = phys_t{se} + frac.se;
	return result;
}

chunk tile::to_chunk() const {
	chunk result;
	result.ne = ne / chunk::tiles_per_chunk;
	result.se = se / chunk::tiles_per_chunk;
	return result;
}

tile tile_delta::to_tile() {
	tile result;
	result.ne = this->ne;
	result.se = this->se;
	return result;
}

tile_delta tile::get_pos_on_chunk() const {
	tile_delta result;

	result.ne = ne % chunk::tiles_per_chunk;
	result.se = se % chunk::tiles_per_chunk;
	return result;
}

} // namespace coord
} // namespace openage
