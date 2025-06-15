// Copyright 2017-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>

#include "util/fixed_point.h"


namespace openage::time {

/**
 * Defines the type that is used as time index.
 * it has to implement all basic mathematically operations.
 */
using time_t = util::FixedPoint<int64_t, 16>;

/**
 * Defines the type that is used for time durations.
 * Same as time_t, but unsigned to cover the whole range of time_t.
 */
using time_duration_t = util::FixedPoint<uint64_t, 16>;

/**
 * Minimum time value.
 */
static constexpr time_t TIME_MIN = std::numeric_limits<time_t>::min();

/**
 * Maximum time value.
 */
static constexpr time_t TIME_MAX = std::numeric_limits<time_t>::max();

/**
 * Zero time value (start of simulation).
 */
static constexpr time_t TIME_ZERO = time_t::zero();

} // namespace openage::time
