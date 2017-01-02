// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "timing.h"

#include <chrono>
#include <cstring>

#include "../error/error.h"

namespace openage {
namespace timing {

time_nsec_t get_real_time() {
	std::chrono::system_clock::time_point t;
	t = std::chrono::system_clock::now();
	auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t.time_since_epoch()).count();
	return static_cast<time_nsec_t>(ns);
}

time_nsec_t get_monotonic_time() {
	std::chrono::steady_clock::time_point t;
	t = std::chrono::steady_clock::now();
	auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t.time_since_epoch()).count();
	return static_cast<time_nsec_t>(ns);
}

}} // namespace openage::timing
