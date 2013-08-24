#include "fps.h"

#include <cmath>

#include <SDL2/SDL.h>

namespace openage {
namespace util {

void FrameCounter::frame() {
	unsigned frame_timestamp = SDL_GetTicks();
	unsigned frame_length = frame_timestamp - lastframe_timestamp;
	if (frame_length <= 1) {
		//just to avoid division by zero or negative errors (clock adjustments etc...)
		//if you can render frames in under 1ms, you don't need accurate FPS.
		frame_length = 1;
	}
	lastframe_timestamp = frame_timestamp;

	//frames that lie 0.5 seconds in the past have 1/e of the weight
	float previous_frames_weight_adjustment = exp((-0.001 / 0.5) * frame_length);

	frame_length_sum_weighted *= previous_frames_weight_adjustment;
	frame_length_sum_weighted += frame_length;

	frame_count_weighted *= previous_frames_weight_adjustment;
	frame_count_weighted += 1;

	if (frame_length_sum_weighted <= 0 || frame_count_weighted <= 0) {
		fps = 0.0;
	} else {
		fps = 1000.0 * frame_count_weighted / frame_length_sum_weighted;
	}
}

} //namespace util
} //namespace openage
