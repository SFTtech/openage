// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "heuristics.h"

#include <cmath>


namespace openage {
namespace path {

cost_t manhattan_cost(const coord::phys3 &start, const coord::phys3 &end) {
	coord::phys_t dx = std::abs(start.ne - end.ne);
	coord::phys_t dy = std::abs(start.se - end.se);
	return static_cast<cost_t>(dx + dy);
}

cost_t chebyshev_cost(const coord::phys3 &start, const coord::phys3 &end) {
	coord::phys_t dx = start.ne - end.ne;
	coord::phys_t dy = start.se - end.se;
	return static_cast<cost_t>(std::max(dx, dy));
}

cost_t euclidean_cost(const coord::phys3 &start, const coord::phys3 &end) {
	coord::phys_t dx = start.ne - end.ne;
	coord::phys_t dy = start.se - end.se;
	return static_cast<cost_t>(std::hypot(dx, dy));
}

} // namespace path
} // namespace openage
