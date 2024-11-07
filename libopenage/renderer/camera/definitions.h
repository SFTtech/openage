// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <eigen3/Eigen/Dense>
#include <limits>

#include "renderer/camera/boundaries.h"

namespace openage::renderer::camera {

/**
 * Camera direction (= where it looks at).
 * Uses a dimetric perspective like in AoE with the (fixed) angles
 *    yaw   = -135 degrees
 *    pitch = -30 degrees
 */
static const Eigen::Vector3f CAM_DIRECTION{
	-1 * (std::sqrt(6.f) / 4),
	-0.5f,
	-1 * (std::sqrt(6.f) / 4),
};

/**
 * Camera up vector.
 */
static const Eigen::Vector3f CAM_UP{0.0f, 1.0f, 0.0f};

/**
 * Default near distance of the camera.
 *
 * Determines how close objects can be to the camera before they are not rendered anymore.
 */
static constexpr float DEFAULT_NEAR_DISTANCE = 0.01f;

/**
 * Default far distance of the camera.
 *
 * Determines how far objects can be from the camera before they are not rendered anymore.
 */
static constexpr float DEFAULT_FAR_DISTANCE = 100.0f;

/**
 * Default zoom level of the camera.
 */
static constexpr float DEFAULT_ZOOM = 1.0f;

/**
 * Maximum zoom out level of the camera.
 */
static constexpr float DEFAULT_MAX_ZOOM_OUT = 64.0f;

/**
 * Default zoom ratio.
 *
 * This adjusts the zoom level of the camera to the size of sprites in the game,
 * so that 1 pixel in a sprite == 1 pixel on the screen.
 *
 * 1.0f / 49 is the default value for the AoE2 sprites.
 */
static constexpr float DEFAULT_ZOOM_RATIO = 1.0f / 49;

static constexpr CameraBoundaries DEFAULT_CAM_BOUNDARIES{
	std::numeric_limits<float>::lowest(),
	std::numeric_limits<float>::max(),
	std::numeric_limits<float>::lowest(),
	std::numeric_limits<float>::max(),
	std::numeric_limits<float>::lowest(),
	std::numeric_limits<float>::max()};

/**
 * Constant values for the camera bounds (based on current fix terrain grid of 20x20).
 * TODO: Make boundaries dynamic based on map size.
 */
static constexpr float X_BOUND_MIN = 12.25f;
static constexpr float X_BOUND_MAX = 32.25f;
static constexpr float Y_BOUND_MIN = 0.0f;
static constexpr float Y_BOUND_MAX = 20.0f;
static constexpr float Z_BOUND_MIN = -8.25f;
static constexpr float Z_BOUND_MAX = 12.25f;

} // namespace openage::renderer::camera
