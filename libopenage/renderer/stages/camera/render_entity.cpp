// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#pragma once

#include "render_entity.h"

namespace openage::renderer::camera {

void RenderEntity::update(const CameraBoundaries& boundaries, const time::time_t time)
{
	std::unique_lock lock{this->mutex};

	this->camera_boundaries = camera_boundaries;
	this->last_update = time;
	this->changed = true;
}

const CameraBoundaries &RenderEntity::get_camera_boundaries() {
	return this->camera_boundaries;
}
}