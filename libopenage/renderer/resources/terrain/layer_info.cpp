// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "layer_info.h"

#include <algorithm>
#include <utility>

#include "renderer/resources/frame_timing.h"
#include "time/time.h"


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
	frame_timing{nullptr} {
	// set frame timings by calculating when they appear in the animation sequence
	auto frame_count = this->frames.size();
	time::time_t t = 0;
	std::vector<time::time_t> frame_timing;
	for (size_t i = 0; i < frame_count; ++i) {
		frame_timing.push_back(t);
		t += this->time_per_frame;
	}
	auto total_time = (frame_count - 1) * this->time_per_frame + this->replay_delay;
	this->frame_timing = std::make_shared<FrameTiming>(total_time, std::move(frame_timing));
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
	return this->frames.at(idx);
}

const std::shared_ptr<FrameTiming> &TerrainLayerInfo::get_frame_timing() const {
	return this->frame_timing;
}


} // namespace openage::renderer::resources
