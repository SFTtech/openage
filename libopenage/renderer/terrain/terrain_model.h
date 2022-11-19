// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <eigen3/Eigen/Dense>
#include <memory>

namespace openage::renderer::terrain {

// TODO: Dimetric/Orthographic version
static const Eigen::Matrix4f proj_matrix{
	{1, 0, 0, 0},
	{0, 1, 0, 0},
	{0, 0, 1, 0},
	{0, 0, 0, 1}};

class TerrainRenderModel {
public:
	TerrainRenderModel();
	~TerrainRenderModel() = default;

	Eigen::Matrix4f get_model_matrix();
	Eigen::Matrix4f get_view_matrix();
	static const Eigen::Matrix4f get_proj_matrix();
};

} // namespace openage::renderer::terrain