// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include <tuple>

#include "boundaries.h"


namespace openage::renderer::camera {

bool CameraBoundaries::operator==(const CameraBoundaries &rhs) {
	return (std::tie(x_min, x_max, y_min, y_max, z_min, z_max) == std::tie(rhs.x_min, rhs.x_max, rhs.y_min, rhs.y_max, rhs.z_min, rhs.z_max));
}

} // namespace openage::renderer::camera
