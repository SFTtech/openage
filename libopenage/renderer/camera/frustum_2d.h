// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <eigen3/Eigen/Dense>

#include "util/vector.h"

namespace openage::renderer::camera {

/**
 * Frustum for culling objects outside of a camera view in 2D screen space.
 * This frustum object should be used for sprite culling as sprites do not exist in 3D world space.
 * They are directly projected and reszied to the screen space.
 *
 * Used for frustum culling (https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling)
 * in the renderer.
 */
class Frustum2d {
public:
	/**
	 * Create a new 2D frustum.
	 *
	 * @param viewport_size Size of the camera viewport (width x height).
	 * @param view_matrix View matrix of the camera.
	 * @param projection_matrix Projection matrix of the camera.
	 * @param zoom_factor Zoom factor of the camera.
	 */
	Frustum2d(const util::Vector2s &viewport_size,
	          const Eigen::Matrix4f &view_matrix,
	          const Eigen::Matrix4f &projection_matrix,
	          const float zoom_factor);

	/**
	 * Update the frustum with new camera parameters.
	 *
	 * @param viewport_size Size of the camera viewport (width x height).
	 * @param view_matrix View matrix of the camera.
	 * @param projection_matrix Projection matrix of the camera.
	 * @param zoom_factor Zoom factor of the camera.
	 */
	void update(const util::Vector2s &viewport_size,
	            const Eigen::Matrix4f &view_matrix,
	            const Eigen::Matrix4f &projection_matrix,
	            const float zoom_factor);

	/**
	 * Check if a scene object is inside the frustum.
	 *
	 * @param scene_pos 3D scene coordinates.
	 * @param scalefactor Scale factor of the animation.
	 * @param boundaries Boundaries of the animation (in pixels): left, right, top, bottom.
	 *
	 * @return true if the object is inside the frustum, false otherwise.
	 */
	bool in_frustum(const Eigen::Vector3f &scene_pos,
	                const float scalefactor,
	                const Eigen::Vector4i &boundaries) const;

private:
	/**
	 * Camera transformation matrix.
	 *
	 * Pre-calculated from view and projection matrix.
	 */
	Eigen::Matrix4f transform_matrix;

	/**
	 * Viewport size of the camera (width x height).
	 */
	Eigen::Vector2f inv_viewport_size;

	/**
	 * Zoom factor of the camera.
	 */
	float inv_zoom_factor;
};

} // namespace openage::renderer::camera
