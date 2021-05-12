// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#include "layer_info.h"

namespace openage::renderer::resources {

LayerInfo::LayerInfo(const display_mode mode,
                     const size_t position,
                     std::vector<AngleInfo> &angles,
                     const float time_per_frame = 0.0,
                     const float replay_delay = 0.0) :
	mode{mode},
	position{position},
	time_per_frame{time_per_frame},
	replay_delay{replay_delay},
	angles{angles} {}

display_mode LayerInfo::get_display_mode() const {
	return this->mode;
}

size_t LayerInfo::get_position() const {
	return this->position;
}

float LayerInfo::get_time_per_frame() const {
	return this->time_per_frame;
}

float LayerInfo::get_replay_delay() const {
	return this->replay_delay;
}

const AngleInfo &LayerInfo::get_angle(size_t idx) const {
	return this->angles[idx];
}

} // namespace openage::renderer::resources
