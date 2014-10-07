#include "fps.h"

#include <math.h>

#include <SDL2/SDL.h>

namespace openage {
namespace util {

FrameCounter::FrameCounter()
	:
	fps(0),
	count(0),
	msec_lastframe(0),
	frame_count_weighted(0),
	frame_length_sum_weighted(0),
	lastframe_timestamp(0) {
}

FrameCounter::~FrameCounter() {}


void FrameCounter::frame() {
	//get milliseconds since sdl startup
	unsigned frame_timestamp = SDL_GetTicks();

	this->msec_lastframe = frame_timestamp - lastframe_timestamp;
	if (this->msec_lastframe <= 1) {
		//just to avoid division by zero or negative errors (clock adjustments etc...)
		//if you can render frames in under 1ms, you don't need accurate FPS.
		this->msec_lastframe = 1;
	}
	this->lastframe_timestamp = frame_timestamp;

	//frames that lie 0.5 seconds in the past have 1/e of the weight
	float previous_frames_weight_adjustment = exp((-0.001 / 0.5) * this->msec_lastframe);

	this->frame_length_sum_weighted *= previous_frames_weight_adjustment;
	this->frame_length_sum_weighted += this->msec_lastframe;

	this->frame_count_weighted *= previous_frames_weight_adjustment;
	this->frame_count_weighted += 1;

	if (this->frame_length_sum_weighted <= 0 || this->frame_count_weighted <= 0) {
		fps = 0.0;
	} else {
		fps = 1000.0 * this->frame_count_weighted / this->frame_length_sum_weighted;
	}

	count += 1;
}

} //namespace util
} //namespace openage
