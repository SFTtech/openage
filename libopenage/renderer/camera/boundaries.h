// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

namespace openage::renderer::camera {

/**
 * Defines boundaries for the camera's view.
 */
struct CameraBoundaries {
	/// The minimum boundary for the camera's X-coordinate.
	float x_min;
	/// The maximum boundary for the camera's X-coordinate.
	float x_max;
	/// The minimum boundary for the camera's Y-coordinate.
	float y_min;
	/// The maximum boundary for the camera's Y-coordinate.
	float y_max;
	/// The minimum boundary for the camera's Z-coordinate.
	float z_min;
	/// The maximum boundary for the camera's Z-coordinate.
	float z_max;
};

} // namespace openage::renderer::camera
