// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <eigen3/Eigen/Dense>
#include <memory>

namespace openage::renderer::terrain {

// Dimetric/Orthographic projection
// Basically rotates the scene by 45 degrees to the right
// and by 60 degrees along the vertical axis
// TODO: Move to camera?
static const Eigen::Matrix4f proj_matrix{
	{1.0f, -1.0f, 0.0f, 0.0f},
	{0.5f, 0.5f, 0.0f, 0.0f},
	{0.0f, 0.0f, 1.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 1.0f}};

class TerrainRenderModel {
public:
	TerrainRenderModel();
	~TerrainRenderModel() = default;

	Eigen::Matrix4f get_model_matrix();
	Eigen::Matrix4f get_view_matrix();
	static const Eigen::Matrix4f get_proj_matrix();
};

} // namespace openage::renderer::terrain