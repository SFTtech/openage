// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <eigen3/Eigen/Dense>

#include "util/hash.h"
#include "util/misc.h"

#include "coord/coord_nese.gen.h"
#include "coord/coord_neseup.gen.h"
#include "coord/declarations.h"
#include "coord/phys.h"

namespace openage {

namespace renderer::camera {
class Camera;
}

namespace coord {


/*
 * Gameworld-aligned coordinate systems.
 * See doc/code/coordinate-systems.md for more information.
 */


struct scene2_delta : CoordNeSeRelative<scene_t, scene2, scene2_delta> {
	using CoordNeSeRelative<scene_t, scene2, scene2_delta>::CoordNeSeRelative;

	double length() const;
	scene2_delta normalize(double length = 1) const;

	// coordinate conversions
	scene3_delta to_scene3() const;
	phys2_delta to_phys2() const;

	// renderer conversions
	Eigen::Vector3f to_world_space() const;
	float to_angle(const coord::scene2_delta &other = {-1, 1}) const; // TODO: other = cam direction?
};

struct scene2 : CoordNeSeAbsolute<scene_t, scene2, scene2_delta> {
	using CoordNeSeAbsolute<scene_t, scene2, scene2_delta>::CoordNeSeAbsolute;

	double distance(scene2 other) const;

	// coordinate conversions
	scene3 to_scene3(phys_t altitude = 0) const;
	phys2 to_phys2() const;

	// renderer conversions
	Eigen::Vector3f to_world_space() const;
};

struct scene3_delta : CoordNeSeUpRelative<scene_t, scene3, scene3_delta> {
	using CoordNeSeUpRelative<scene_t, scene3, scene3_delta>::CoordNeSeUpRelative;

	double length() const;
	scene3_delta normalize(double length = 1) const;

	// coordinate conversions
	scene2_delta to_scene2() const;
	phys3_delta to_phys3() const;

	// renderer conversions
	Eigen::Vector3f to_world_space() const;
	float to_angle(const coord::scene2_delta &other = {-1, 1}) const; // TODO: other = cam direction?
};

struct scene3 : CoordNeSeUpAbsolute<scene_t, scene3, scene3_delta> {
	using CoordNeSeUpAbsolute<scene_t, scene3, scene3_delta>::CoordNeSeUpAbsolute;

	// coordinate conversions
	scene2 to_scene2() const;
	phys3 to_phys3() const;

	// renderer conversions
	Eigen::Vector3f to_world_space() const;
};


} // namespace coord
} // namespace openage

namespace std {

template <>
struct hash<openage::coord::scene3> {
	size_t operator()(const openage::coord::scene3 &pos) const {
		size_t hash = openage::util::type_hash<openage::coord::scene3>();
		hash = openage::util::hash_combine(hash, std::hash<openage::coord::scene_t>{}(pos.ne));
		hash = openage::util::hash_combine(hash, std::hash<openage::coord::scene_t>{}(pos.se));
		hash = openage::util::hash_combine(hash, std::hash<openage::coord::scene_t>{}(pos.up));
		return hash;
	}
};

} // namespace std
