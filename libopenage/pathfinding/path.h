// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "coord/tile.h"
#include "pathfinding/types.h"
#include "time/time.h"


namespace openage::path {

/**
 * Path request for the pathfinder.
 */
struct PathRequest {
	/// ID of the grid to use for pathfinding.
	size_t grid_id;
	/// Start position of the path.
	coord::tile start;
	/// Target position of the path.
	coord::tile target;
	/// Time the request was made.
	const time::time_t time;
};

/**
 * Path found by the pathfinder.
 */
struct Path {
	/// ID of the grid to used for pathfinding.
	size_t grid_id;
	/// Status
	PathResult status;
	/// Waypoints of the path.
	/// First waypoint is the start position of the path request.
	/// Last waypoint is the target position of the path request.
	std::vector<coord::tile> waypoints;
};

} // namespace openage::path
