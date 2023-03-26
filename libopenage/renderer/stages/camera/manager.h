// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

namespace openage::renderer::camera {

class Camera;

enum class MoveDirection {
	NONE = 0x0000,
	LEFT = 0x0001,
	RIGHT = 0x0010,
	TOP = 0x0100,
	BOTTOM = 0x1000,
};

enum class ZoomDirection {
	NONE = 0x0000,
	IN = 0x0001,
	OUT = 0x0010,
};

/**
 * Manages updates to the camera that happen every frame.
 *
 * This may be used for smooth camera movements that are not based on
 * input events.
 */
class CameraManager {
public:
	CameraManager(const std::shared_ptr<renderer::camera::Camera> &camera);
	~CameraManager() = default;

	/**
	 * Update the camera parameters.
	 */
	void update();

	/**
     * Set the move directions of the camera.
     *
     * @param directions Bitfield of the move directions.
     */
	void set_move_directions(unsigned char directions);

	/**
     * Set the zoom direction of the camera.
     *
     * @param direction Zoom direction.
     */
	void set_zoom_direction(unsigned char direction);

	/**
     * Set the move speed of the camera.
     *
     * @param speed Speed of the camera.
     */
	void set_move_speed(float speed);

	/**
     * Set the zoom speed of the camera.
     *
     * @param speed Speed of the camera.
     */
	void set_zoom_speed(float speed);

private:
	/**
     * Camera.
     */
	std::shared_ptr<renderer::camera::Camera> camera;

	/**
     * Bitfield of the current move directions.
     */
	unsigned char move_directions;

	/**
     * Bitfield of the current zoom direction.
     */
	unsigned char zoom_direction;

	/**
     * Move speed of the camera.
     */
	float move_speed;

	/**
     * Zoom speed of the camera.
     */
	float zoom_speed;
};
} // namespace openage::renderer::camera
