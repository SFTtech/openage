// Copyright 2013-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "timer.h"

namespace openage {
namespace util {

class FrameCounter {
public:
	FrameCounter();
	~FrameCounter();

	/** to be called each time a frame has been completed */
	void frame();

	/** auto-updated to always contain the current FPS value */
	float fps;

	/** contains the number of completed frames */
	unsigned count;

	/** nanoseconds used for the last frame */
	int64_t nsec_lastframe;

private:
	float frame_count_weighted;
	float frame_length_sum_weighted;

	Timer frame_timer;
};

}} // openage::util
