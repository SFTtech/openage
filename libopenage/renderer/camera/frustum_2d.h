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
	 * @param zoom Zoom of the camera.
	 */
	Frustum2d(const util::Vector2s &viewport_size,
	          const Eigen::Matrix4f &view_matrix,
	          const Eigen::Matrix4f &projection_matrix,
	          const float zoom);

	/**
	 * Update the frustum with new camera parameters.
	 *
	 * @param viewport_size Size of the camera viewport (width x height).
	 * @param view_matrix View matrix of the camera.
	 * @param projection_matrix Projection matrix of the camera.
	 * @param zoom Zoom of the camera.
	 */
	void update(const util::Vector2s &viewport_size,
	            const Eigen::Matrix4f &view_matrix,
	            const Eigen::Matrix4f &projection_matrix,
	            const float zoom);

	/**
	 * Check if a scene object is inside the frustum.
	 *
	 * @param scene_pos 3D scene coordinates.
	 * @param model_matrix Model matrix of the object.
	 * @param scalefactor Scale factor of the animation.
	 * @param boundaries Boundaries of the animation (in pixels): left, right, top, bottom.
	 *
	 * @return true if the object is inside the frustum, false otherwise.
	 */
	bool in_frustum(const Eigen::Vector3f &scene_pos,
	                const Eigen::Matrix4f &model_matrix,
	                const float scalefactor,
	                const util::Vector4i &boundaries) const;

private:
	/**
	 * Camera transformation matrix.
	 *
	 * Pre-calculated from view and projection matrix.
	 */
	Eigen::Matrix4f transform_matrix;

	/**
	 * Size of a pixel (width x height) in clip space.
	 *
	 * Uses normalized device coordinates (NDC) for the pixel size.
	 */
	Eigen::Vector2f pixel_size_ndc;

	/**
	 * Zoom factor of the camera.
	 */
	float inv_zoom_factor;
};

} // namespace openage::renderer::camera
