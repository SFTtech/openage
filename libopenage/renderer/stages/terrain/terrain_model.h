// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <eigen3/Eigen/Dense>
#include <memory>

namespace openage::renderer {

namespace terrain {

class TerrainRenderModel {
public:
	TerrainRenderModel();
	~TerrainRenderModel() = default;

	Eigen::Matrix4f get_model_matrix();
};

} // namespace terrain
} // namespace openage::renderer