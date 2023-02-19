// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "frame_info.h"

namespace openage::renderer::resources {

TerrainFrameInfo::TerrainFrameInfo(size_t texture_idx,
                                   size_t subtexture_idx,
                                   size_t priority,
                                   std::optional<size_t> blend_mode) :
	texture_idx{texture_idx},
	subtexture_idx{subtexture_idx},
	priority{priority},
	blend_mode{blend_mode} {}

size_t TerrainFrameInfo::get_texture_idx() const {
	return this->texture_idx;
}

size_t TerrainFrameInfo::get_subtexture_idx() const {
	return this->subtexture_idx;
}

size_t TerrainFrameInfo::get_priority() const {
	return this->priority;
}

std::optional<size_t> TerrainFrameInfo::get_blend_mode() const {
	return this->blend_mode;
}

} // namespace openage::renderer::resources
