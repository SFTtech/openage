// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "frame_info.h"

namespace openage::renderer::resources {

FrameInfo::FrameInfo(const size_t texture_idx,
                     const size_t subtexture_idx) :
	texture_idx{texture_idx},
	subtexture_idx{subtexture_idx} {}

size_t FrameInfo::get_texture_idx() const {
	return this->texture_idx;
}

size_t FrameInfo::get_subtexture_idx() const {
	return this->subtexture_idx;
}

} // namespace openage::renderer::resources
