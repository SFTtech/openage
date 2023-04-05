// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "blendpattern_info.h"

namespace openage::renderer::resources {

BlendPatternInfo::BlendPatternInfo(const float scalefactor,
                                   std::vector<std::shared_ptr<Texture2dInfo>> &textures,
                                   std::vector<blending_mask> &masks) :
	scalefactor{scalefactor},
	textures{textures} {
	for (auto mask : masks) {
		auto mask_val = std::make_pair(mask.texture_id, mask.subtex_id);
		this->masks.emplace(mask.directions, mask_val);
	}
}

float BlendPatternInfo::get_scalefactor() const {
	return this->scalefactor;
}

size_t BlendPatternInfo::get_texture_count() const {
	return this->textures.size();
}

const std::shared_ptr<Texture2dInfo> &BlendPatternInfo::get_texture(size_t idx) const {
	return this->textures.at(idx);
}

size_t BlendPatternInfo::get_mask_count() const {
	return this->masks.size();
}

std::vector<std::pair<size_t, size_t>> BlendPatternInfo::get_masks(char directions) const {
	std::vector<std::pair<size_t, size_t>> result{};

	auto lookup = this->masks.find(directions);
	if (lookup != this->masks.end()) {
		result.push_back(lookup->second);
		return result;
	}
	// else find best matching masks
	for (auto mask : this->masks) {
		if (mask.first & directions) {
			result.push_back(mask.second);
		}
	}

	return result;
}

} // namespace openage::renderer::resources
