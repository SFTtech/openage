// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <eigen3/Eigen/Dense>

#include "util/vector.h"


namespace openage::renderer::camera {

/**
 * Frustum for culling objects outside of a camera viewcone in 3D space. The frustum
 * is defined by 6 planes (top, bottom, right, left, far, near) that define the boundaries
 * of the viewing volume of the camera.
 *
 * Used for frustum culling (https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling)
 * in the renderer.
 *
 * As the openage camera uses orthographic projection, the frustum is a box, i.e. plane opposite
 * to each other are parallel.
 */
class Frustum3d {
public:
	/**
	 * Create a new frustum.
	 *
	 * @param viewport_size Size of the viewport (width x height).
	 * @param near_distance Near distance of the frustum.
	 * @param far_distance Far distance of the frustum.
	 * @param camera_pos Scene position of the camera.
	 * @param cam_direction Direction the camera is looking at.
	 * @param up_direction Up direction of the camera.
	 * @param zoom_factor Zoom factor of the camera.
	 */
	Frustum3d(const util::Vector2s &viewport_size,
	          const float near_distance,
	          const float far_distance,
	          const Eigen::Vector3f &camera_pos,
	          const Eigen::Vector3f &cam_direction,
	          const Eigen::Vector3f &up_direction,
	          const float zoom_factor);

	/**
	 * Update this frustum with the new camera parameters.
	 *
	 * @param viewport_size Size of the viewport (width x height).
	 * @param near_distance Near distance of the frustum.
	 * @param far_distance Far distance of the frustum.
	 * @param camera_pos Scene position of the camera.
	 * @param cam_direction Direction the camera is looking at.
	 * @param up_direction Up direction of the camera.
	 * @param zoom_factor Zoom factor of the camera.
	 */
	void update(const util::Vector2s &viewport_size,
	            const float near_distance,
	            const float far_distance,
	            const Eigen::Vector3f &camera_pos,
	            const Eigen::Vector3f &cam_direction,
	            const Eigen::Vector3f &up_direction,
	            const float zoom_factor);

	/**
	 * Check whether a point in the scene is inside the frustum.
	 *
	 * @param scene_pos 3D scene coordinates.
	 *
	 * @return true if the point is inside the frustum, else false.
	 */
	bool in_frustum(const Eigen::Vector3f &scene_pos) const;

private:
	/**
	 * Normal vector of the top face.
	 */
	Eigen::Vector3f top_face_normal;

	/**
	 * Normal vector of the bottom face.
	 */
	Eigen::Vector3f bottom_face_normal;

	/**
	 * Normal vector of the right face.
	 */
	Eigen::Vector3f right_face_normal;

	/**
	 * Normal vector of the left face.
	 */
	Eigen::Vector3f left_face_normal;

	/**
	 * Normal vector of the far face.
	 */
	Eigen::Vector3f far_face_normal;

	/**
	 * Normal vector of the near face.
	 */
	Eigen::Vector3f near_face_normal;

	/**
	 * Shortest distance from the top face to the scene origin.
	 */
	float top_face_distance;

	/**
	 * Shortest distance from the bottom face to the scene origin.
	 */
	float bottom_face_distance;

	/**
	 * Shortest distance from the right face to the scene origin.
	 */
	float right_face_distance;

	/**
	 * Shortest distance from the left face to the scene origin.
	 */
	float left_face_distance;

	/**
	 * Shortest distance from the far face to the scene origin.
	 */
	float far_face_distance;

	/**
	 * Shortest distance from the near face to the scene origin.
	 */
	float near_face_distance;
};
} // namespace openage::renderer::camera
