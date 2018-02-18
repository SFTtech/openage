// Copyright 2016-2018 the openage authors. See copying.md for legal info.

#pragma once

#include "../util/misc.h"

#include "declarations.h"
#include "coord_nese.gen.h"
#include "coord_neseup.gen.h"

namespace openage {

class Terrain;

namespace coord {


/*
 * Gameworld-aligned coordinate systems.
 * See doc/code/coordinate-systems.md for more information.
 */


struct phys2_delta : CoordNeSeRelative<phys_t, phys2, phys2_delta> {
	using CoordNeSeRelative<phys_t, phys2, phys2_delta>::CoordNeSeRelative;

	double length() const;
	phys2_delta normalize(double length=1) const;
	phys3_delta to_phys3() const;
};

struct phys2 : CoordNeSeAbsolute<phys_t, phys2, phys2_delta> {
	using CoordNeSeAbsolute<phys_t, phys2, phys2_delta>::CoordNeSeAbsolute;

	tile to_tile() const;
	phys3 to_phys3(const Terrain &terrain, phys_t altitude=0) const;

	double distance(phys2 other) const;
};

struct phys3_delta : CoordNeSeUpRelative<phys_t, phys3, phys3_delta> {
	using CoordNeSeUpRelative<phys_t, phys3, phys3_delta>::CoordNeSeUpRelative;

	// there's no converter to convert phys3 deltas to tile3 deltas because
	// phys3_delta{0.5, 0, 0} might result in tile3_delta{0, 0, 0} or
	// tile3_delta{1, 0, 0} depending on the absolute position.
	// we don't allow ambiguous conversions.

	double length() const;
	phys3_delta normalize(double length=1) const;

	camgame_delta to_camgame(const CoordManager &mgr) const;
	phys2_delta to_phys2() const;
};

struct phys3 : CoordNeSeUpAbsolute<phys_t, phys3, phys3_delta> {
	using CoordNeSeUpAbsolute<phys_t, phys3, phys3_delta>::CoordNeSeUpAbsolute;

	tile3 to_tile3() const;
	tile to_tile() const;
	phys2 to_phys2() const;

	camgame to_camgame(const CoordManager &mgr) const;
	viewport to_viewport(const CoordManager &mgr) const;
	camhud to_camhud(const CoordManager &mgr) const;
};


} // namespace coord
} // namespace openage

namespace std {

template<>
struct hash<openage::coord::phys3> {
	size_t operator ()(const openage::coord::phys3 &pos) const {
		return (
			openage::util::rol<size_t, 2>(hash<openage::coord::phys_t>{}(pos.ne)) ^
			openage::util::rol<size_t, 1>(hash<openage::coord::phys_t>{}(pos.se)) ^
			                              hash<openage::coord::phys_t>{}(pos.up)
		);
	}
};

} // namespace std
