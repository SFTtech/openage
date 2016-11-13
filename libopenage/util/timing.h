// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>

namespace openage {

/**
 * Type to store time in nanoseconds.
 */
using time_nsec_t = uint64_t;


namespace timing {

/**
 * returns a monotonically-increasing time that returns the number of
 * nanoseconds since an unspecified event.
 *
 * not influenced by system time changes.
 */
time_nsec_t get_monotonic_time();

/**
 * returns the number of nanoseconds since the UNIX epoch.
 */
time_nsec_t get_real_time();

}} // namespace openage::timing
