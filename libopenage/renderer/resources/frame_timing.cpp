// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "frame_timing.h"

#include <algorithm>
#include <compare>

#include "error/error.h"
#include "log/message.h"

#include "util/fixed_point.h"


namespace openage::renderer::resources {

FrameTiming::FrameTiming(const time::time_t &total_length,
                         const std::vector<keyframe_t> &&keyframes) :
	keyframes{keyframes},
	total_length{total_length} {
	if (this->keyframes.size() < 1) [[unlikely]] {
		throw Error(ERR << "Frame timing sequence must have at least one keyframe.");
	}
}

void FrameTiming::set_total_length(const time::time_t &total_length) {
	this->total_length = total_length;
}

void FrameTiming::insert(const time::time_t &time) {
	for (auto it = this->keyframes.begin(); it != this->keyframes.end(); ++it) {
		if (*it >= time) {
			this->keyframes.insert(it, time);
			return;
		}
	}
}

size_t FrameTiming::get_frame(const time::time_t &time) const {
	return search_frame(time);
}

size_t FrameTiming::get_frame(const time::time_t &time, const time::time_t &start) const {
	time::time_t offset = time - start;
	if (this->total_length == 0) {
		// modulo would fail here so return early
		return 0;
	}

	time::time_t mod = offset % this->total_length;
	return this->get_frame(mod);
}

size_t FrameTiming::size() const {
	return this->keyframes.size();
}

size_t FrameTiming::search_frame(const time::time_t &time) const {
	size_t left = 0;
	size_t right = this->keyframes.size();
	size_t mid = 0;

	// rightmost binary search
	while (left < right) {
		mid = (left + right) / 2;

		if (time < this->keyframes.at(mid)) {
			right = mid;
		}
		else {
			left = mid + 1;
		}
	}

	return right - 1;
}


} // namespace openage::renderer::resources
