// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "../util/misc.h"
#include "coord_nese.gen.h"
#include "coord_neseup.gen.h"

namespace openage {
namespace coord {

using tile_t = int64_t;


struct tile_delta;
struct tile;
struct tile3_delta;
struct tile3;


struct tile_delta : CoordNeSeRelative<tile_t, tile, tile_delta> {
	using CoordNeSeRelative<tile_t, tile, tile_delta>::CoordNeSeRelative;
};

struct tile : CoordNeSeAbsolute<tile_t, tile, tile_delta> {
	using CoordNeSeAbsolute<tile_t, tile, tile_delta>::CoordNeSeAbsolute;
};

struct tile3_delta : CoordNeSeUpRelative<tile_t, tile3, tile3_delta> {
	using CoordNeSeUpRelative<tile_t, tile3, tile3_delta>::CoordNeSeUpRelative;
};

struct tile3 : CoordNeSeUpAbsolute<tile_t, tile3, tile3_delta> {
	using CoordNeSeUpAbsolute<tile_t, tile3, tile3_delta>::CoordNeSeUpAbsolute;
};


} // namespace coord
} // namespace openage

namespace std {

template<>
struct hash<openage::coord::tile> {
	size_t operator ()(const openage::coord::tile &pos) const {
		size_t nehash = hash<openage::coord::tile_t>{}(pos.ne);
		size_t sehash = hash<openage::coord::tile_t>{}(pos.se);
		return openage::util::rol<size_t, 1>(nehash) ^ sehash;
	}
};

} // namespace std
