// Copyright 2014-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "pathfinding/legacy/path.h"

namespace openage {
namespace path::legacy {

/**
 * function pointer type for distance estimation functions.
 */
using heuristic_t = cost_old_t (*)(const coord::phys3 &start, const coord::phys3 &end);

/**
 * Manhattan distance cost estimation.
 * @returns the sum of the x and y difference.
 */
cost_old_t manhattan_cost(const coord::phys3 &start, const coord::phys3 &end);

/**
 * Chebyshev distance cost estimation.
 * @returns y or x difference, whichever is higher.
 */
cost_old_t chebyshev_cost(const coord::phys3 &start, const coord::phys3 &end);

/**
 * Euclidean distance cost estimation.
 * @returns the hypotenuse length of the rectangular triangle formed.
 */
cost_old_t euclidean_cost(const coord::phys3 &start, const coord::phys3 &end);

/**
 * Squared euclidean distance cost estimation.
 * @returns the square of the hypotenuse length of the rectangular triangle formed.
 */
cost_old_t euclidean_squared_cost(const coord::phys3 &start, const coord::phys3 &end);

/**
 * Calculate euclidean distance from a already calculated squared euclidean distance
 */
cost_old_t euclidean_squared_to_euclidean_cost(const cost_old_t euclidean_squared_value);

} // namespace path::legacy
} // namespace openage
