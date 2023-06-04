// Copyright 2016-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "../util/hash.h"
#include "../util/misc.h"

#include "coord_nese.gen.h"
#include "coord_neseup.gen.h"
#include "declarations.h"

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
	phys2_delta normalize(double length = 1) const;
	phys3_delta to_phys3() const;
	scene2_delta to_scene2() const;
};

struct phys2 : CoordNeSeAbsolute<phys_t, phys2, phys2_delta> {
	using CoordNeSeAbsolute<phys_t, phys2, phys2_delta>::CoordNeSeAbsolute;

	tile to_tile() const;
	phys3 to_phys3(const Terrain &terrain, phys_t altitude = 0) const;
	scene2 to_scene2() const;

	double distance(phys2 other) const;
};

struct phys3_delta : CoordNeSeUpRelative<phys_t, phys3, phys3_delta> {
	using CoordNeSeUpRelative<phys_t, phys3, phys3_delta>::CoordNeSeUpRelative;

	// there's no converter to convert phys3 deltas to tile3 deltas because
	// phys3_delta{0.5, 0, 0} might result in tile3_delta{0, 0, 0} or
	// tile3_delta{1, 0, 0} depending on the absolute position.
	// we don't allow ambiguous conversions.

	double length() const;
	phys3_delta normalize(double length = 1) const;

	[[deprecated]] camgame_delta to_camgame(const CoordManager &mgr) const;
	phys2_delta to_phys2() const;
	scene3_delta to_scene3() const;
};

struct phys3 : CoordNeSeUpAbsolute<phys_t, phys3, phys3_delta> {
	using CoordNeSeUpAbsolute<phys_t, phys3, phys3_delta>::CoordNeSeUpAbsolute;

	tile3 to_tile3() const;
	tile to_tile() const;
	phys2 to_phys2() const;
	scene3 to_scene3() const;

	[[deprecated]] camgame to_camgame(const CoordManager &mgr) const;
	[[deprecated]] viewport to_viewport(const CoordManager &mgr) const;
	[[deprecated]] camhud to_camhud(const CoordManager &mgr) const;
};


} // namespace coord
} // namespace openage

namespace std {

template <>
struct hash<openage::coord::phys3> {
	size_t operator()(const openage::coord::phys3 &pos) const {
		size_t hash = openage::util::type_hash<openage::coord::phys3>();
		hash = openage::util::hash_combine(hash, std::hash<openage::coord::phys_t>{}(pos.ne));
		hash = openage::util::hash_combine(hash, std::hash<openage::coord::phys_t>{}(pos.se));
		hash = openage::util::hash_combine(hash, std::hash<openage::coord::phys_t>{}(pos.up));
		return hash;
	}
};

} // namespace std
