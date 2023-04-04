// Copyright 2013-2023 the openage authors. See copying.md for legal info.

#include "texture_subinfo.h"


namespace openage::renderer::resources {

Texture2dSubInfo::Texture2dSubInfo(uint32_t x,
                                   uint32_t y,
                                   uint32_t w,
                                   uint32_t h,
                                   uint32_t cx,
                                   uint32_t cy,
                                   uint32_t atlas_width,
                                   uint32_t atlas_height) :
	x(x),
	y(y),
	w(w),
	h(h),
	cx(cx),
	cy(cy),
	tile_coords{
		static_cast<float>(x) / atlas_width,
		static_cast<float>(y) / atlas_height,
		static_cast<float>(w) / atlas_width,
		static_cast<float>(h) / atlas_height,
	},
	anchor_coords{
		static_cast<float>(cx) / atlas_width,
		static_cast<float>(cy) / atlas_height} {}

const Eigen::Vector4f &Texture2dSubInfo::get_tile_coords() const {
	return this->tile_coords;
}

const Eigen::Vector2f &Texture2dSubInfo::get_anchor_coords() const {
	return this->anchor_coords;
}

} // namespace openage::renderer::resources
