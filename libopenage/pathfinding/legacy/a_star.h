// Copyright 2014-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "heuristics.h"
#include "path.h"

namespace openage {

namespace path::legacy {

/**
 * path between two static points
 */
Path to_point(coord::phys3 start,
              coord::phys3 end,
              std::function<bool(const coord::phys3 &)> passable);

/**
 * path to nearest object with lambda
 */
Path find_nearest(coord::phys3 start,
                  std::function<bool(const coord::phys3 &)> valid_end,
                  std::function<bool(const coord::phys3 &)> passable);

/**
 * finds a path between two endpoints
 * @param start the starting tile coords
 * @param end the ending tile coords
 * @param heuristic the heuristic for evaluating cost
 * @param passable lambda to decide which terrain is passable
 * @return path between the given tiles
 */
Path a_star(coord::phys3 start,
            std::function<bool(const coord::phys3 &)> valid_end,
            std::function<cost_old_t(const coord::phys3 &)> heuristic,
            std::function<bool(const coord::phys3 &)> passable);

} // namespace path::legacy
} // namespace openage
