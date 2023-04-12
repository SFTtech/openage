// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "scene.h"

#include "coord/coordmanager.h"
#include "coord/pixel.h"
#include "coord/tile.h"
#include "util/math.h"


namespace openage::coord {

double scene2_delta::length() const {
	return std::hypot(this->ne.to_double(), this->se.to_double());
}

scene2_delta scene2_delta::normalize(double length) const {
	return *this * (length / this->length());
}

scene3_delta scene2_delta::to_scene3() const {
	return scene3_delta{this->ne, this->se, 0};
}

phys2_delta scene2_delta::to_phys2() const {
	return phys2_delta(this->ne, this->se);
}

Eigen::Vector3f scene2_delta::to_vector() const {
	return Eigen::Vector3f(-this->se.to_float(), 0.0f, this->ne.to_float());
}

double scene2::distance(scene2 other) const {
	return (*this - other).length();
}

scene3 scene2::to_scene3(phys_t altitude) const {
	return scene3{this->ne, this->se, altitude};
}

phys2 scene2::to_phys2() const {
	return phys2(this->ne, this->se);
}

Eigen::Vector3f scene2::to_vector() const {
	return Eigen::Vector3f(-this->se.to_float(), 0.0f, this->ne.to_float());
}

double scene3_delta::length() const {
	return math::hypot3(this->ne.to_double(), this->se.to_double(), this->up.to_double());
}

scene3_delta scene3_delta::normalize(double length) const {
	return *this * (length / this->length());
}

scene2_delta scene3_delta::to_scene2() const {
	return scene2_delta(this->ne, this->se);
}

phys3_delta scene3_delta::to_phys3() const {
	return phys3_delta(this->ne, this->se, this->up);
}

Eigen::Vector3f scene3_delta::to_vector() const {
	return Eigen::Vector3f(-this->se.to_float(), this->up.to_float(), this->ne.to_float());
}

scene2 scene3::to_scene2() const {
	return scene2{this->ne, this->se};
}

phys3 scene3::to_phys3() const {
	return phys3(this->ne, this->se, this->up);
}

Eigen::Vector3f scene3::to_vector() const {
	return Eigen::Vector3f(-this->se.to_float(), this->up.to_float(), this->ne.to_float());
}

} // namespace openage::coord
