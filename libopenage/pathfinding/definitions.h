// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <limits>

#include "pathfinding/types.h"


namespace openage::path {

/**
 * Init value for a cells in the cost grid.
 *
 * Should not be used for actual costs.
 */
constexpr cost_t COST_INIT = 0;

/**
 * Minimum possible cost for a passable cell in the cost grid.
 */
constexpr cost_t COST_MIN = 1;

/**
 * Maximum possible cost for a passable cell in the cost grid.
 */
constexpr cost_t COST_MAX = 254;

/**
 * Cost value for an impassable cell in the cost grid.
 */
constexpr cost_t COST_IMPASSABLE = 255;


/**
 * Unreachable value for a cells in the integration grid.
 */
constexpr integrate_t INTEGRATE_UNREACHABLE = std::numeric_limits<integrate_t>::max();

} // namespace openage::path
