// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "frustum_2d.h"


namespace openage::renderer::camera {

Frustum2d::Frustum2d(const util::Vector2s &viewport_size,
                     const Eigen::Matrix4f &view_matrix,
                     const Eigen::Matrix4f &projection_matrix,
                     const float zoom) {
	this->update(viewport_size, view_matrix, projection_matrix, zoom);
}

void Frustum2d::update(const util::Vector2s &viewport_size,
                       const Eigen::Matrix4f &view_matrix,
                       const Eigen::Matrix4f &projection_matrix,
                       const float zoom) {
	this->inv_viewport_size = {1.0f / viewport_size[0], 1.0f / viewport_size[1]};
	this->inv_zoom_factor = 1.0f / zoom;

	// calculate the transformation matrix
	this->transform_matrix = projection_matrix * view_matrix;
}

bool Frustum2d::in_frustum(const Eigen::Vector3f &scene_pos,
                           const float scalefactor,
                           const util::Vector4i &boundaries) const {
	// calculate the position of the scene object in screen space
	Eigen::Vector4f clip_pos = this->transform_matrix * Eigen::Vector4f(scene_pos[0], scene_pos[1], scene_pos[2], 1.0f);
	float x_ndc = clip_pos[0];
	float y_ndc = clip_pos[1];

	float zoom_scale = scalefactor * this->inv_zoom_factor;

	// Scale the boundaries by the zoom factor
	Eigen::Vector4f scaled_boundaries{boundaries[0] * zoom_scale * this->inv_viewport_size[0],
	                                  boundaries[1] * zoom_scale * this->inv_viewport_size[0],
	                                  boundaries[2] * zoom_scale * this->inv_viewport_size[1],
	                                  boundaries[3] * zoom_scale * this->inv_viewport_size[1]};
	float left_bound = scaled_boundaries[0];
	float right_bound = scaled_boundaries[1];
	float top_bound = scaled_boundaries[2];
	float bottom_bound = scaled_boundaries[3];

	// check if the object boundaries are inside the frustum
	if (x_ndc - left_bound >= 1.0f) {
		return false;
	}
	if (x_ndc + right_bound <= -1.0f) {
		return false;
	}
	if (y_ndc + top_bound <= -1.0f) {
		return false;
	}
	if (y_ndc - bottom_bound >= 1.0f) {
		return false;
	}

	return true;
}

} // namespace openage::renderer::camera
