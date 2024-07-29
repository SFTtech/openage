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
	this->pixel_size_ndc = {2.0f / viewport_size[0], 2.0f / viewport_size[1]};
	this->inv_zoom_factor = 1.0f / zoom;

	// calculate the transformation matrix
	this->transform_matrix = projection_matrix * view_matrix;
}

bool Frustum2d::in_frustum(const Eigen::Vector3f &scene_pos,
                           const Eigen::Matrix4f &model_matrix,
                           const float scalefactor,
                           const util::Vector4i &boundaries) const {
	// calculate the position of the scene object in screen space
	Eigen::Vector4f clip_pos = this->transform_matrix * model_matrix * scene_pos.homogeneous();
	float x_ndc = clip_pos[0];
	float y_ndc = clip_pos[1];

	float zoom_scale = scalefactor * this->inv_zoom_factor;

	// Scale the boundaries by the zoom factor and the pixel size
	float left_bound = boundaries[0] * zoom_scale * this->pixel_size_ndc[0];
	float right_bound = boundaries[1] * zoom_scale * this->pixel_size_ndc[0];
	float top_bound = boundaries[2] * zoom_scale * this->pixel_size_ndc[1];
	float bottom_bound = boundaries[3] * zoom_scale * this->pixel_size_ndc[1];

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
