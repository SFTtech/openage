// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "layer_info.h"

#include "renderer/resources/animation/angle_info.h"

namespace openage::renderer::resources {

LayerInfo::LayerInfo(std::vector<std::shared_ptr<AngleInfo>> &angles,
                     const display_mode mode,
                     const size_t position,
                     const float time_per_frame,
                     const float replay_delay) :
	mode{mode},
	position{position},
	time_per_frame{time_per_frame},
	replay_delay{replay_delay},
	angles{angles},
	frame_timing{std::make_shared<curve::DiscreteMod<size_t>>(nullptr, 0)} {
	if (this->angles.size() > 0) {
		// create a curve for looking up which frames should
		// be displayed at which time inside layer
		auto frame_count = this->angles[0]->get_frame_count();
		curve::time_t t = 0;
		for (size_t i = 0; i < frame_count; ++i) {
			frame_timing->set_insert(t, i);
			t += this->time_per_frame;
		}
		t = t - this->time_per_frame + this->replay_delay;
		frame_timing->set_insert(t, 0);
	}
}

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

size_t LayerInfo::get_angle_count() const {
	return this->angles.size();
}

const std::shared_ptr<AngleInfo> &LayerInfo::get_angle(size_t idx) const {
	return this->angles.at(idx);
}

const std::shared_ptr<curve::DiscreteMod<size_t>> &LayerInfo::get_frame_timing() const {
	return this->frame_timing;
}


} // namespace openage::renderer::resources
