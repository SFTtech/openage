// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "layer_info.h"

#include <algorithm>
#include <utility>

#include "renderer/resources/animation/angle_info.h"
#include "renderer/resources/frame_timing.h"
#include "time/time.h"


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
	frame_timing{nullptr} {
	if (this->angles.size() > 0) {
		// set frame timings by calculating when they appear in the animation sequence
		auto frame_count = this->angles[0]->get_frame_count();
		time::time_t t = 0;
		std::vector<time::time_t> keyframes;
		for (size_t i = 0; i < frame_count; ++i) {
			keyframes.push_back(t);
			t += this->time_per_frame;
		}
		auto total_time = (frame_count - 1) * this->time_per_frame + this->replay_delay;
		this->frame_timing = std::make_shared<FrameTiming>(total_time, std::move(keyframes));
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

const std::shared_ptr<AngleInfo> &LayerInfo::get_direction_angle(float direction) const {
	if (this->angles.size() == 1) {
		// angle covers all directions
		return this->get_angle(0);
	}

	// clamp to possible degrees values
	direction = std::clamp(direction, 0.0f, 360.0f);

	// special case for front facing angle because values wrap
	// around at 360 degrees, e.g. 337,5 to 22,5
	if (direction > this->angles.at(0)->get_angle_start()
	    or direction < this->angles.at(1)->get_angle_start()) {
		return this->get_angle(0);
	}

	// check if angles with index = angles.size() - 2 match
	// since they are ordered, we only need one comparison
	// (check if the NEXT angle start is larger than the supplied
	// direction)
	for (size_t i = 1; i < this->angles.size(); ++i) {
		if (direction < this->angles.at(i)->get_angle_start()) {
			return this->get_angle(i - 1);
		}
	}

	// last angle in the list is the only one remaining that could match
	return this->get_angle(this->angles.size() - 1);
}

const std::shared_ptr<FrameTiming> &LayerInfo::get_frame_timing() const {
	return this->frame_timing;
}


} // namespace openage::renderer::resources
