// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "layer_info.h"

namespace openage::renderer::resources {

TerrainLayerInfo::TerrainLayerInfo(const std::vector<std::shared_ptr<TerrainFrameInfo>> &frames,
                                   const terrain_display_mode mode,
                                   const size_t position,
                                   const float time_per_frame,
                                   const float replay_delay) :
	mode{mode},
	position{position},
	time_per_frame{time_per_frame},
	replay_delay{replay_delay},
	frames{frames},
	frame_timing{std::make_shared<curve::DiscreteMod<size_t>>(nullptr, 0)} {
	// create a curve for looking up which frames should
	// be displayed at which time inside layer
	auto frame_count = this->frames.size();
	curve::time_t t = 0;
	for (size_t i = 0; i < frame_count; ++i) {
		frame_timing->set_insert(t, i);
		t += this->time_per_frame;
	}
	t = t - this->time_per_frame + this->replay_delay;
	frame_timing->set_insert(t, 0);
}

terrain_display_mode TerrainLayerInfo::get_display_mode() const {
	return this->mode;
}

size_t TerrainLayerInfo::get_position() const {
	return this->position;
}

float TerrainLayerInfo::get_time_per_frame() const {
	return this->time_per_frame;
}

float TerrainLayerInfo::get_replay_delay() const {
	return this->replay_delay;
}

size_t TerrainLayerInfo::get_frame_count() const {
	return this->frames.size();
}

const std::shared_ptr<TerrainFrameInfo> &TerrainLayerInfo::get_frame(size_t idx) const {
	return this->frames[idx];
}

const std::shared_ptr<curve::DiscreteMod<size_t>> &TerrainLayerInfo::get_frame_timing() const {
	return this->frame_timing;
}


} // namespace openage::renderer::resources
