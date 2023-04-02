// Copyright 2013-2023 the openage authors. See copying.md for legal info.

#include "texture_subinfo.h"


namespace openage::renderer::resources {

Texture2dSubInfo::Texture2dSubInfo(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t cx, uint32_t cy) :
	x(x), y(y), w(w), h(h), cx(cx), cy(cy) {}

} // namespace openage::renderer::resources
