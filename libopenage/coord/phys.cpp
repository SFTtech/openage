// Copyright 2016-2023 the openage authors. See copying.md for legal info.

#include "phys.h"

#include "coord/coordmanager.h"
#include "coord/pixel.h"
#include "coord/scene.h"
#include "coord/tile.h"
#include "terrain/terrain.h"
#include "util/math.h"
#include "util/math_constants.h"


namespace openage::coord {


double phys2_delta::length() const {
	return std::hypot(this->ne.to_double(), this->se.to_double());
}


phys2_delta phys2_delta::normalize(double length) const {
	return *this * (length / this->length());
}


phys3_delta phys2_delta::to_phys3() const {
	return phys3_delta{this->ne, this->se, 0};
}


scene2_delta phys2_delta::to_scene2() const {
	return scene2_delta(this->ne, this->se);
}

phys_angle_t phys2_delta::to_angle(const coord::phys2_delta &other) const {
	// TODO: Using floats here will result in inaccuracies.
	//       Better use a fixed point version of atan2.
	auto det = other.ne.to_float() * this->se.to_float() - this->ne.to_float() * other.se.to_float();
	auto dot = this->ne.to_float() * other.ne.to_float() + this->se.to_float() * other.se.to_float();

	auto angle = std::atan2(det, dot) * 180 / math::PI;
	if (angle < 0) {
		angle += 360;
	}

	return phys_angle_t::from_float(angle);
}


double phys2::distance(phys2 other) const {
	return (*this - other).length();
}


tile phys2::to_tile() const {
	return tile{this->ne.to_int(), this->se.to_int()};
}

phys3 phys2::to_phys3(phys_t up) const {
	return phys3(this->ne, this->se, up);
}

scene2 phys2::to_scene2() const {
	return scene2(this->ne, this->se);
}


[[deprecated]] phys3 phys2::to_phys3(const Terrain & /* terrain */, phys_t altitude) const {
	// TODO: once terrain elevations have been implemented,
	//       query the terrain elevation at {ne, se}.
	phys_t elevation = 0;

	return phys3{this->ne, this->se, elevation + altitude};
}


double phys3_delta::length() const {
	return math::hypot3(this->ne.to_double(), this->se.to_double(), this->up.to_double());
}


phys3_delta phys3_delta::normalize(double length) const {
	return *this * (length / this->length());
}


phys2_delta phys3_delta::to_phys2() const {
	return phys2_delta{this->ne, this->se};
}


scene3_delta phys3_delta::to_scene3() const {
	return scene3_delta{this->ne, this->se, this->up};
}

phys_angle_t phys3_delta::to_angle(const coord::phys2_delta &other) const {
	// TODO: Using floats here will result in inaccuracies.
	//       Better use a fixed point version of atan2.
	auto det = other.ne.to_float() * this->se.to_float() - this->ne.to_float() * other.se.to_float();
	auto dot = this->ne.to_float() * other.ne.to_float() + this->se.to_float() * other.se.to_float();

	auto angle = std::atan2(det, dot) * 180 / math::PI;
	if (angle < 0) {
		angle += 360;
	}

	return phys_angle_t::from_float(angle);
}


[[deprecated]] camgame_delta phys3_delta::to_camgame(const CoordManager &mgr) const {
	// apply transformation matrix to phys3_delta, to get 'scaled':
	//                   (ne)
	//  (x) = (+1 +1 +0) (se)
	//  (y) = (+1 -1 +1) (up)
	phys_t x = phys_t{this->ne} * (+1) + this->se * (+1) + this->up * (+0);
	phys_t y = phys_t{this->ne} * (+1) + this->se * (-1) + this->up * (+1);

	// add scaling (w/2 for x, h/2 for y)
	return camgame_delta{
		static_cast<camgame_delta::elem_t>((x * (mgr.tile_size.x / 2)).to_int()),
		static_cast<camgame_delta::elem_t>((y * (mgr.tile_size.y / 2)).to_int())};
}


tile3 phys3::to_tile3() const {
	return tile3{this->ne.to_int(), this->se.to_int(), this->up.to_int()};
}


tile phys3::to_tile() const {
	return this->to_tile3().to_tile();
}


phys2 phys3::to_phys2() const {
	return phys2{this->ne, this->se};
}


scene3 phys3::to_scene3() const {
	return scene3{this->ne, this->se, this->up};
}


[[deprecated]] camgame phys3::to_camgame(const CoordManager &mgr) const {
	return (*this - mgr.camgame_phys).to_camgame(mgr) + camgame{0, 0};
}


[[deprecated]] viewport phys3::to_viewport(const CoordManager &mgr) const {
	return this->to_camgame(mgr).to_viewport(mgr);
}


[[deprecated]] camhud phys3::to_camhud(const CoordManager &mgr) const {
	return this->to_viewport(mgr).to_camhud(mgr);
}


} // namespace openage::coord
