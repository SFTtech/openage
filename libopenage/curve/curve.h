// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>

#include "../util/fixed_point.h"


namespace openage::curve {

/**
 * Defines the type that is used as time index.
 * it has to implement all basic mathematically operations.
 */
using time_t = util::FixedPoint<int64_t, 16>;

} // openage::curve
