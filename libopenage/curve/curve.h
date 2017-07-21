// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>

namespace openage {
namespace curve {

/**
 * Defines the type that is used as time index.
 * it has to implement all basic mathematically operations.
 *
 * pxd:
 *     ctypedef double curve_time_t
 */
typedef int64_t curve_time_t;

}} // openage::curve
