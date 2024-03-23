// Copyright 2016-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "../util/hash.h"
#include "../util/misc.h"
#include "coord_nese.gen.h"
#include "coord_neseup.gen.h"
#include "declarations.h"

namespace openage {
namespace coord {

/*
 * Gameworld tile-related coordinate systems.
 * See doc/code/coordinate-systems.md for more information.
 */


struct tile_delta : CoordNeSeRelative<tile_t, tile, tile_delta> {
	using CoordNeSeRelative<tile_t, tile, tile_delta>::CoordNeSeRelative;

	phys2_delta to_phys2() const;
	phys3_delta to_phys3(tile_t up = 0) const;
};

struct tile : CoordNeSeAbsolute<tile_t, tile, tile_delta> {
	using CoordNeSeAbsolute<tile_t, tile, tile_delta>::CoordNeSeAbsolute;

	// coordinate conversions
	/**
	 * adds an UP component to the coordinate.
	 * the resulting UP component will be 'altitude' tiles above ground
	 * elevation.
	 */
	tile3 to_tile3(tile_t up = 0) const;
	phys2 to_phys2() const;
	phys3 to_phys3(tile_t up = 0) const;
	chunk to_chunk() const;
	tile_delta get_pos_on_chunk() const;
};

struct tile3_delta : CoordNeSeUpRelative<tile_t, tile3, tile3_delta> {
	using CoordNeSeUpRelative<tile_t, tile3, tile3_delta>::CoordNeSeUpRelative;

	// coordinate conversions
	// simply discards the UP component of the coordinate delta.
	constexpr tile_delta to_tile() const {
		return tile_delta{this->ne, this->se};
	}
	phys3_delta to_phys3() const;
};

struct tile3 : CoordNeSeUpAbsolute<tile_t, tile3, tile3_delta> {
	using CoordNeSeUpAbsolute<tile_t, tile3, tile3_delta>::CoordNeSeUpAbsolute;

	// coordinate conversions
	// simply discards the UP component of the coordinate.
	constexpr tile to_tile() const {
		return tile{this->ne, this->se};
	}
	phys2 to_phys2() const;
	phys3 to_phys3() const;
};


} // namespace coord
} // namespace openage

namespace std {

template <>
struct hash<openage::coord::tile> {
	size_t operator()(const openage::coord::tile &pos) const {
		size_t hash = openage::util::type_hash<openage::coord::tile>();
		hash = openage::util::hash_combine(hash, std::hash<openage::coord::tile_t>{}(pos.ne));
		hash = openage::util::hash_combine(hash, std::hash<openage::coord::tile_t>{}(pos.se));
		return hash;
	}
};

} // namespace std
