// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_PATHFINDING_HEURISTICS_H_
#define OPENAGE_PATHFINDING_HEURISTICS_H_

#include "path.h"

namespace openage {
namespace path {

/**
 * function pointer type for distance estimation functions.
 */
using heuristic_t = cost_t (*)(const coord::phys3 &start, const coord::phys3 &end);

/**
 * Manhattan distance cost estimation.
 * @returns the sum of the x and y difference.
 */
cost_t manhattan_cost(const coord::phys3 &start, const coord::phys3 &end);

/**
 * Chebyshev distance cost estimation.
 * @returns y or x difference, whichever is higher.
 */
cost_t chebyshev_cost(const coord::phys3 &start, const coord::phys3 &end);

/**
 * Euclidean distance cost estimation.
 * @returns the hypotenuse length of the rectangular triangle formed.
 */
cost_t euclidean_cost(const coord::phys3 &start, const coord::phys3 &end);

} // namespace path
} // namespace openage

#endif
