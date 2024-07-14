// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cmath>
#include <cstddef>
#include <memory>

#include <eigen3/Eigen/Dense>

#include "coord/pixel.h"
#include "coord/scene.h"
#include "util/vector.h"

namespace openage::renderer::camera {

/**
 * Frustum for a camera.
 *
 * Used for frustum culling (https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling)
 * in the renderer.
 */
class Frustum {
public:
	/**
	 * Create a new frustum.
	 */
	Frustum();

	/**
	 * Update this frustum with the new camera parameters.
	 *
	 * @param viewport_size Size of the viewport (width x height).
	 * @param near_distance Near distance of the frustum.
	 * @param far_distance Far distance of the frustum.
	 * @param scene_pos Scene position of the camera.
	 * @param cam_direction Direction the camera is looking at.
	 * @param up_direction Up direction of the camera.
	 * @param real_zoom Zoom factor of the camera.
	 */
	void update(util::Vector2s &viewport_size,
	            float near_distance,
	            float far_distance,
	            Eigen::Vector3f &scene_pos,
	            Eigen::Vector3f cam_direction,
	            Eigen::Vector3f up_direction,
	            float real_zoom);

	/**
	 * Check whether a point in the scene is inside the frustum.
	 *
	 * @param scene_pos 3D scene coordinates.
	 *
	 * @return true if the point is inside the frustum, else false.
	 */
	bool in_frustum(Eigen::Vector3f &scene_pos) const;

private:
	Eigen::Vector3f top_face_normal;
	Eigen::Vector3f bottom_face_normal;
	Eigen::Vector3f right_face_normal;
	Eigen::Vector3f left_face_normal;
	Eigen::Vector3f far_face_normal;
	Eigen::Vector3f near_face_normal;

	float top_face_distance;
	float bottom_face_distance;
	float right_face_distance;
	float left_face_distance;
	float far_face_distance;
	float near_face_distance;
};
} // namespace openage::renderer::camera
