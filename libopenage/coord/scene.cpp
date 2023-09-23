// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "scene.h"

#include <numbers>

#include "coord/pixel.h"
#include "coord/tile.h"
#include "util/math.h"
#include "util/math_constants.h"


namespace openage::coord {

/**
 * Ratio for converting the scene coordinate UP direction to OpenGL/Vulkan
 * coordinates, i.e. translating ingame "height" to displayed height.
 * The ratio is 1 / sqrt(8), which is an approximation of the ratio used in AoE2.
 *
 * @return Multiplier for the UP direction.
 */
constexpr float up_ratio() {
	// approx. 1.0 / sqrt(8)
	return 0.353553391f;
}


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


Eigen::Vector3f scene2_delta::to_world_space() const {
	return Eigen::Vector3f(this->se.to_float(), 0.0f, -this->ne.to_float());
}

float scene2_delta::to_angle(const coord::scene2_delta &other) const {
	auto det = other.ne.to_float() * this->se.to_float() - this->ne.to_float() * other.se.to_float();
	auto dot = this->ne.to_float() * other.ne.to_float() + this->se.to_float() * other.se.to_float();

	auto angle = std::atan2(det, dot) * 180 / math::PI;
	if (angle < 0) {
		angle += 360;
	}

	return angle;
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


Eigen::Vector3f scene2::to_world_space() const {
	return Eigen::Vector3f(this->se.to_float(), 0.0f, -this->ne.to_float());
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


Eigen::Vector3f scene3_delta::to_world_space() const {
	return Eigen::Vector3f(this->se.to_float(), this->up.to_float() * up_ratio(), -this->ne.to_float());
}

float scene3_delta::to_angle(const coord::scene2_delta &other) const {
	return this->to_scene2().to_angle(other);
}

scene2 scene3::to_scene2() const {
	return scene2{this->ne, this->se};
}


phys3 scene3::to_phys3() const {
	return phys3(this->ne, this->se, this->up);
}


Eigen::Vector3f scene3::to_world_space() const {
	return Eigen::Vector3f(this->se.to_float(), this->up.to_float() * up_ratio(), -this->ne.to_float());
}

} // namespace openage::coord
