// Copyright 2013-2024 the openage authors. See copying.md for legal info.

#include "texture_subinfo.h"


namespace openage::renderer::resources {

Texture2dSubInfo::Texture2dSubInfo(int32_t x,
                                   int32_t y,
                                   uint32_t w,
                                   uint32_t h,
                                   int32_t cx,
                                   int32_t cy,
                                   uint32_t atlas_width,
                                   uint32_t atlas_height) :
	pos{x, y},
	size{w, h},
	anchor_pos{cx, cy},
	tile_params{
		static_cast<float>(x) / atlas_width,
		static_cast<float>(y) / atlas_height,
		static_cast<float>(w) / atlas_width,
		static_cast<float>(h) / atlas_height,
	},
	anchor_params{
		w - 2 * cx,
		-h + 2 * cy} {}

const Eigen::Vector2i &Texture2dSubInfo::get_pos() const {
	return this->pos;
}

const Eigen::Vector2<uint32_t> &Texture2dSubInfo::get_size() const {
	return this->size;
}

const Eigen::Vector2i &Texture2dSubInfo::get_anchor_pos() const {
	return this->anchor_pos;
}

const Eigen::Vector4f &Texture2dSubInfo::get_subtex_coords() const {
	return this->tile_params;
}

const Eigen::Vector2i &Texture2dSubInfo::get_anchor_params() const {
	return this->anchor_params;
}

} // namespace openage::renderer::resources
