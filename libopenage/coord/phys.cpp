// Copyright 2016-2023 the openage authors. See copying.md for legal info.

#include "phys.h"

#include "coord/pixel.h"
#include "coord/scene.h"
#include "coord/tile.h"
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

} // namespace openage::coord
