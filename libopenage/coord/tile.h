// Copyright 2016-2019 the openage authors. See copying.md for legal info.

#pragma once

#include "declarations.h"
#include "coord_nese.gen.h"
#include "coord_neseup.gen.h"
#include "../util/misc.h"
#include "../util/hash.h"
#include <typeindex>

namespace openage {

class Terrain;

namespace coord {

class CoordManager;


/*
 * Gameworld tile-related coordinate systems.
 * See doc/code/coordinate-systems.md for more information.
 */


struct tile_delta : CoordNeSeRelative<tile_t, tile, tile_delta> {
	using CoordNeSeRelative<tile_t, tile, tile_delta>::CoordNeSeRelative;
};

struct tile : CoordNeSeAbsolute<tile_t, tile, tile_delta> {
	using CoordNeSeAbsolute<tile_t, tile, tile_delta>::CoordNeSeAbsolute;

	/**
	 * adds an UP component to the coordinate.
	 * the resulting UP component will be 'altitude' tiles above ground
	 * elevation.
	 */
	tile3 to_tile3(const Terrain &terrain, tile_t altitude=0) const;
	phys2 to_phys2() const;
	phys3 to_phys3(const Terrain &terrain, tile_t altitude=0) const;
	camgame to_camgame(const CoordManager &mgr, const Terrain &terrain, tile_t altitude=0) const;
	viewport to_viewport(const CoordManager &mgr, const Terrain &terrain, tile_t altitude=0) const;
	chunk to_chunk() const;
	tile_delta get_pos_on_chunk() const;
};

struct tile3_delta : CoordNeSeUpRelative<tile_t, tile3, tile3_delta> {
	using CoordNeSeUpRelative<tile_t, tile3, tile3_delta>::CoordNeSeUpRelative;

	// simply discards the UP component of the coordinate delta.
	constexpr tile_delta to_tile() const {
		return tile_delta{this->ne, this->se};
	}
};

struct tile3 : CoordNeSeUpAbsolute<tile_t, tile3, tile3_delta> {
	using CoordNeSeUpAbsolute<tile_t, tile3, tile3_delta>::CoordNeSeUpAbsolute;

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

template<>
struct hash<openage::coord::tile> {
	size_t operator ()(const openage::coord::tile &pos) const {
		size_t hash = std::hash<std::type_index>()(std::type_index(typeid(openage::coord::tile)));
		hash = openage::util::hash_combine(hash, std::hash<openage::coord::tile_t>{}(pos.ne));
		hash = openage::util::hash_combine(hash, std::hash<openage::coord::tile_t>{}(pos.se));
		return hash;
	}
};

} // namespace std
