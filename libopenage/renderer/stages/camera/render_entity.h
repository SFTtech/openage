// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#pragma once

#include "renderer/camera/boundaries.h"
#include "gamestate/definitions.h"
#include "renderer/stages/render_entity.h"


namespace openage::renderer::camera {

class RenderEntity final : public renderer::RenderEntity {
public:
	RenderEntity();
	~RenderEntity() = default;

	void update(const CameraBoundaries& camera_boundaries, const time::time_t time = 0.0);

	const CameraBoundaries& get_camera_boundaries();
	


private:
	CameraBoundaries camera_boundaries;
};

}