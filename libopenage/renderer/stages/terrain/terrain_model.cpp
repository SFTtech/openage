// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#include "terrain_model.h"

namespace openage::renderer::terrain {

TerrainRenderModel::TerrainRenderModel() {
}

Eigen::Matrix4f TerrainRenderModel::get_model_matrix() {
	// TODO: Needs input from engine
	auto transform = Eigen::Affine3f::Identity();
	transform.prescale(Eigen::Vector3f(0.2f, 0.2f, 0.2f));
	transform.pretranslate(Eigen::Vector3f(-1.0f, -1.0f, -1.0f));
	return transform.matrix();
}

Eigen::Matrix4f TerrainRenderModel::get_view_matrix() {
	// TODO: Camera
	return Eigen::Matrix4f::Identity();
}

const Eigen::Matrix4f TerrainRenderModel::get_proj_matrix() {
	return proj_matrix;
	// return Eigen::Matrix4f::Identity();
}

} // namespace openage::renderer::terrain