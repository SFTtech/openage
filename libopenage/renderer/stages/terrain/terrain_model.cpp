// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#include "terrain_model.h"

namespace openage::renderer::terrain {

TerrainRenderModel::TerrainRenderModel() {
}

Eigen::Matrix4f TerrainRenderModel::get_model_matrix() {
	// TODO: Needs input from engine
	auto transform = Eigen::Affine3f::Identity();
	return transform.matrix();
}

} // namespace openage::renderer::terrain