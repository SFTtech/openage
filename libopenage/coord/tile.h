// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_TILE_H_
#define OPENAGE_COORD_TILE_H_

#include <functional>

#include "decl.h"
#include "ne_se_up_coord.h"
#include "phys2.h"
#include "../util/misc.h"

namespace openage {
namespace coord {

struct tile : public absolute_ne_se_up_coord<tile, tile_delta, tile_t, 2> {
	tile() = default;
	tile(tile_t ne, tile_t se);

	tile3 to_tile3(tile_t up = 0) const;
	phys2 to_phys2(phys2_delta frac = {settings::phys_per_tile / 2, settings::phys_per_tile / 2}) const;
	chunk to_chunk() const;
	tile_delta get_pos_on_chunk() const;
};

struct tile_delta : public relative_ne_se_up_coord<tile, tile_delta, tile_t, 2>{
	tile_delta() = default;
	tile_delta(tile_t ne, tile_t se);

	tile3_delta to_tile3(tile_t up = 0) const;
	tile to_tile();
};

} // namespace coord
} // namespace openage


namespace std {
template<>
struct hash<openage::coord::tile> {
	size_t operator ()(const openage::coord::tile& pos) const {
		size_t nehash = hash<openage::coord::tile_t>{}(pos.ne);
		size_t sehash = hash<openage::coord::tile_t>{}(pos.se);
		return openage::util::rol<size_t, 1>(nehash) ^ sehash;
	}
};
} // namespace std

#endif
