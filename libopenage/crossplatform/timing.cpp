// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "timing.h"

#include <cstring>

#include "../error/error.h"

#ifdef __linux
#include <time.h>
#elif __APPLE__
#include <sys/time.h>
#include <mach/mach_time.h>
#endif

namespace openage {
namespace timing {

#ifdef __linux
int64_t posix_get_time(clockid_t clk_id) {
	struct timespec t;

	if (clock_gettime(clk_id, &t) != 0) {
		throw Error(MSG(err) << "clock_gettime failed: " << strerror(errno));
	};

	return static_cast<int64_t>(t.tv_sec) * 1000000000L + static_cast<int64_t>(t.tv_nsec);
}
#endif


int64_t get_real_time() {
	#ifdef __linux

	// we don't need supreme accuracy; REALTIME_COARSE should suffice.
	return posix_get_time(CLOCK_REALTIME_COARSE);

	#elif __APPLE__

	struct timeval t;
	gettimeofday(&t, NULL);
	return static_cast<int64_t>(t.tv_sec) * 1000000000L + static_cast<int64_t>(t.tv_usec) * 1000L;

	#elif _WIN32
	static_assert(false, "get_real_time not yet implemented for WIN32");
	#else
	static_assert(false, "unknown operating system");
	#endif
}

int64_t get_monotonic_time() {
	#ifdef __linux

	return posix_get_time(CLOCK_MONOTONIC);

	#elif __APPLE__

	int64_t time = mach_absolute_time();

	// memorize the numerator / denominator over function calls.
	static int64_t numerator = 0;
	static int64_t denominator = 0;

	if (!denominator) {
		mach_timebase_info_data_t info;

		if (mach_timebase_info(&info) != 0) {
			throw Error(MSG(err) << "could not determine monotime: " << strerror(errno));
		}

		numerator = info.numer;
		denominator = info.denom;
	}

	// Wheeeeeeee, we need 128-bit arithmetics to avoid overflows!
	// I guess 32-bit mac users are fucked (but I doubt they'd have C++14 anyway)
	return (static_cast<__int128_t>(time) * static_cast<__int128_t>(numerator)) / static_cast<__int128_t>(denominator);

	#elif _WIN32
	static_assert(false, "get_monotonic_time not yet implemented for WIN32");
	#else
	static_assert(false, "unknown operating system");
	#endif
}

}} // namespace openage::timing
