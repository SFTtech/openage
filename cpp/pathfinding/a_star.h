// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_PATHFINDING_A_STAR_H_
#define OPENAGE_PATHFINDING_A_STAR_H_

#include <memory>

#include "../terrain/terrain_object.h"
#include "heuristics.h"
#include "path.h"

namespace openage {
namespace path {

Path to_point(coord::phys3 start,
              coord::phys3 end,
              std::function<bool(const coord::phys3 &)> passable);

Path to_object(openage::TerrainObject *to_move,
               openage::TerrainObject *end);

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
            std::function<cost_t(const coord::phys3 &)> heuristic,
            std::function<bool(const coord::phys3 &)> passable);

} // namespace path
} // namespace openage

#endif
