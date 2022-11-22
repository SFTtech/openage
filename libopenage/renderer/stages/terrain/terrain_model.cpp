// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#include "terrain_model.h"

namespace openage::renderer::terrain {

TerrainRenderModel::TerrainRenderModel() {
}

Eigen::Matrix4f TerrainRenderModel::get_model_matrix() {
	// TODO: Needs input from engine
	return Eigen::Matrix4f::Identity();
}

Eigen::Matrix4f TerrainRenderModel::get_view_matrix() {
	// TODO: Camera
	return Eigen::Matrix4f::Identity();
}

const Eigen::Matrix4f TerrainRenderModel::get_proj_matrix() {
	return proj_matrix;
}

} // namespace openage::renderer::terrain