// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#include "heuristics.h"

#include <cmath>


namespace openage {
namespace path {

cost_t manhattan_cost(const coord::phys3 &start, const coord::phys3 &end) {
	cost_t dx = std::abs((cost_t)start.ne - end.ne);
	cost_t dy = std::abs((cost_t)start.se - end.se);
	return dx + dy;
}


cost_t costFunction(const coord::phys3& start, const coord::phys3& end,
		bool chebyshev, bool euclidean) {
	cost_t dx = start.ne - end.ne;
	cost_t dy = start.se - end.se;
	if (chebyshev)
		return std::max(dx, dy);

	if (euclidean)
		return std::hypot(dx, dy);

	//Default is Euclidean. Include another if clause if this function is to be used for a new type of cost calculation with similar body 
	return std::hypot(dx, dy)
}

cost_t chebyshev_cost(const coord::phys3 &start, const coord::phys3 &end) {
	return costFunction(start, end, true, false);
}

cost_t euclidean_cost(const coord::phys3 &start, const coord::phys3 &end) {
	return costFunction(start, end, false, true);
}

} // namespace path
} // namespace openage
