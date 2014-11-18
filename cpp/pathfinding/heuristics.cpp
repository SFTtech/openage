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

cost_t chebyshev_cost(const coord::phys3 &start, const coord::phys3 &end) {
	cost_t dx = start.ne - end.ne;
	cost_t dy = start.se - end.se;
	return std::max(dx, dy);
}

cost_t euclidean_cost(const coord::phys3 &start, const coord::phys3 &end) {
	cost_t dx = start.ne - end.ne;
	cost_t dy = start.se - end.se;
	return std::hypot(dx, dy);
}

} // namespace path
} // namespace openage
