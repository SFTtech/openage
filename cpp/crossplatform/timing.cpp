// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "timing.h"

#ifdef __linux
#include <time.h>
#endif

namespace openage {
namespace timing {

#ifdef __linux
int64_t posix_get_time(clockid_t clk_id) {
	struct timespec t;
	clock_gettime(clk_id, &t);
	return ((long) t.tv_sec * 1000000000L) + (long) t.tv_nsec;
}
#endif

int64_t get_real_time() {
	#ifdef __linux
	// we don't need supreme accuracy; REALTIME_COARSE should suffice.
	return posix_get_time(CLOCK_REALTIME_COARSE);
	#elif __APPLE__
	static_assert(false, "get_real_time not yet implemented for Apple");
	#elif WIN32
	static_assert(false, "get_real_time not yet implemented for WIN32");
	#else
	static_assert(false, "unknown operating system");
	#endif
}

int64_t get_monotonic_time() {
	#ifdef __linux
	return posix_get_time(CLOCK_MONOTONIC);
	#elif __APPLE__
	static_assert(false, "get_monotonic_time not yet implemented for Apple");
	#elif WIN32
	static_assert(false, "get_monotonic_time not yet implemented for WIN32");
	#else
	static_assert(false, "unknown operating system");
	#endif
}

}} // namespace openage::timing
