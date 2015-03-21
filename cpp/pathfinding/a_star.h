// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_PATHFINDING_A_STAR_H_
#define OPENAGE_PATHFINDING_A_STAR_H_

#include <memory>

#include "../terrain/terrain_object.h"
#include "heuristics.h"
#include "path.h"

namespace openage {
namespace path {

/**
 * path between two static points
 */
Path to_point(coord::phys3 start,
              coord::phys3 end,
              std::function<bool(const coord::phys3 &)> passable);

/**
 * path between 2 objects, with how close to come to end point
 */
Path to_object(TerrainObject *to_move,
               TerrainObject *end,
               coord::phys_t rad);

/**
 * path to nearest object with lambda
 */
Path find_nearest(coord::phys3 start,
                  std::function<bool(const coord::phys3 &)> valid_end,
                  std::function<bool(const coord::phys3 &)> passable);

/**
 * search outwards to find an object and return object
 */
openage::TerrainObject *find_nearest(TerrainObject *to_move,
            std::function<bool(const TerrainObject *)> valid_end);

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
