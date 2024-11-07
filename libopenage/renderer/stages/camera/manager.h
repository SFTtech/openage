// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <utility>

#include "renderer/camera/camera.h"

namespace openage::renderer {
class UniformBufferInput;

namespace camera {

class Camera;

enum class MoveDirection {
	NONE = 0x0000,
	LEFT = 0x0001,
	RIGHT = 0x0002,
	FORWARD = 0x0004,
	BACKWARD = 0x0008,
};

enum class ZoomDirection {
	NONE = 0x0000,
	IN = 0x0001,
	OUT = 0x0002,
};

/**
 * Manages per-frame changes to camera parameters.
 *
 * The camera manager operates with directions instead of
 * scene coordinate, which should make it easier to use.
 *
 * There are two update modes:
 *     - Frame updates: Applies only to the current frame (e.g. from a key press).
 *     - Motion updates: Applies to the current frame and all subsequent frames
 *                       until the motion is stopped.
 *
 * TODO: Multiple cameras?
 * TODO: Priority queues (let updates with higher priority override lower ones)?
 *
 */
class CameraManager {
public:
	/**
	 * Create a new camera manager.
	 *
	 * @param camera Camera to manage.
	 * @param camera_boundaries Boundaries for the camera movement in the scene.
	 */
	CameraManager(const std::shared_ptr<renderer::camera::Camera> &camera,
	              const CameraBoundaries &camera_boundaries = DEFAULT_CAM_BOUNDARIES);
	~CameraManager() = default;

	/**
	 * Update the camera position and zoom level.
	 *
	 * Additionally updates the camera uniform buffer with the view and projection
	 * matrices.
	 */
	void update();

	/**
	 * Move into the given direction for the current frame.
	 *
	 * @param direction Move direction.
	 * @param delta Move speed, i.e. a distance multiplier.
	 */
	void move_frame(MoveDirection direction, float speed = 1.0f);

	/**
	 * Zoom into the given direction for the current frame.
	 *
	 * @param direction Zoom direction.
	 * @param delta Zoom speed, i.e. a distance multiplier.
	 */
	void zoom_frame(ZoomDirection direction, float speed = 1.0f);

	/**
	 * Set the move directions of the camera.
	 *
	 * @param directions Bitfield of the move directions.
	 */
	void set_move_motion_dirs(int directions);

	/**
	 * Set the zoom direction of the camera.
	 *
	 * @param direction Zoom direction.
	 */
	void set_zoom_motion_dir(int direction);

	/**
	 * Set the move speed of the camera.
	 *
	 * @param speed Speed of the camera.
	 */
	void set_move_motion_speed(float speed);

	/**
	 * Set the zoom speed of the camera.
	 *
	 * @param speed Speed of the camera.
	 */
	void set_zoom_motion_speed(float speed);

	/**
	 * Set boundaries for camera movement in the scene.
	 *
	 * @param camera_boundaries XYZ boundaries for the camera movement.
	 */
	void set_camera_boundaries(const CameraBoundaries &camera_boundaries);

private:
	/**
	 * Update the camera parameters.
	 */
	void update_motion();

	/**
	 * Update the camera uniform buffer.
	 */
	void update_uniforms();

	/**
	 * Camera.
	 */
	std::shared_ptr<renderer::camera::Camera> camera;

	/**
	 * Bitfield of the current move motion directions.
	 */
	int move_motion_directions;

	/**
	 * Bitfield of the current zoom motion direction.
	 */
	int zoom_motion_direction;

	/**
	 * Move motion speed of the camera.
	 */
	float move_motion_speed;

	/**
	 * Zoom motion speed of the camera.
	 */
	float zoom_motion_speed;

	/**
	 * Uniform buffer input for the camera.
	 */
	std::shared_ptr<renderer::UniformBufferInput> uniforms;

	/**
	 * Camera boundaries for X and Z movement. Contains minimum and maximum values for each axes.
	 */
	CameraBoundaries camera_boundaries;
};

} // namespace camera
} // namespace openage::renderer
