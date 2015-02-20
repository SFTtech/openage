// Copyright 2013-2014 the openage authors. See copying.md for legal info.

#include "fps.h"

#include <math.h>

#include "../crossplatform/timing.h"

namespace openage {
namespace util {

FrameCounter::FrameCounter()
	:
	fps{0},
	count{0},
	nsec_lastframe{0},
	frame_count_weighted{0},
	frame_length_sum_weighted{0},
	frame_timer{false} {
}

FrameCounter::~FrameCounter() {}


void FrameCounter::frame() {
	this->nsec_lastframe = this->frame_timer.getandresetval();
	if (this->nsec_lastframe <= 1) {
		// never can be safe enough
		this->nsec_lastframe = 1;
	}

	// frames that lie 0.5 seconds in the past have 1/e of the weight
	float previous_frames_weight_adjustment = exp((-0.001 / 0.5) * this->nsec_lastframe);

	this->frame_length_sum_weighted *= previous_frames_weight_adjustment;
	this->frame_length_sum_weighted += this->nsec_lastframe;

	this->frame_count_weighted *= previous_frames_weight_adjustment;
	this->frame_count_weighted += 1;

	if (this->frame_length_sum_weighted <= 0 || this->frame_count_weighted <= 0) {
		fps = 0.0;
	} else {
		fps = 1e9 * this->frame_count_weighted / this->frame_length_sum_weighted;
	}

	count += 1;
}

} //namespace util
} //namespace openage
