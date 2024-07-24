// Copyright 2021-2024 the openage authors. See copying.md for legal info.

#include "animation_info.h"

#include "renderer/resources/animation/angle_info.h"
#include "renderer/resources/animation/frame_info.h"
#include "renderer/resources/texture_info.h"


namespace openage::renderer::resources {

Animation2dInfo::Animation2dInfo(const float scalefactor,
                                 std::vector<std::shared_ptr<Texture2dInfo>> &textures,
                                 std::vector<LayerInfo> &layers) :
	scalefactor{scalefactor},
	texture_infos{textures},
	layers{layers},
	max_bounds{} {
	// Calculate max bounds
	for (auto &layer : this->layers) {
		for (size_t i = 0; i < layer.get_angle_count(); ++i) {
			auto &angle = layer.get_angle(i);
			for (size_t j = 0; j < angle->get_frame_count(); ++j) {
				auto &frame = angle->get_frame(j);
				auto tex_idx = frame->get_texture_idx();
				auto subtex_idx = frame->get_subtexture_idx();

				auto &tex = this->texture_infos.at(tex_idx);
				auto &subtex = tex->get_subtex_info(subtex_idx);

				auto subtex_size = subtex.get_size();
				auto anchor_pos = subtex.get_anchor_pos();

				int left_margin = anchor_pos.x();
				int right_margin = subtex_size.x() - anchor_pos.x();
				int top_margin = anchor_pos.y();
				int bottom_margin = subtex_size.y() - anchor_pos.y();

				this->max_bounds[0] = std::max(this->max_bounds[0], left_margin);
				this->max_bounds[1] = std::max(this->max_bounds[1], right_margin);
				this->max_bounds[2] = std::max(this->max_bounds[2], top_margin);
				this->max_bounds[3] = std::max(this->max_bounds[3], bottom_margin);
			}
		}
	}
}

float Animation2dInfo::get_scalefactor() const {
	return this->scalefactor;
}

size_t Animation2dInfo::get_texture_count() const {
	return this->texture_infos.size();
}

const std::shared_ptr<Texture2dInfo> &Animation2dInfo::get_texture(size_t idx) const {
	return this->texture_infos.at(idx);
}

size_t Animation2dInfo::get_layer_count() const {
	return this->layers.size();
}

const LayerInfo &Animation2dInfo::get_layer(size_t idx) const {
	return this->layers.at(idx);
}

const util::Vector4i &Animation2dInfo::get_max_bounds() const {
	return this->max_bounds;
}

const util::Vector2s Animation2dInfo::get_max_size() const {
	return {this->max_bounds[0] + this->max_bounds[1],
	        this->max_bounds[2] + this->max_bounds[3]};
}

} // namespace openage::renderer::resources
