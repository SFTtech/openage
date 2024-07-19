// Copyright 2014-2024 the openage authors. See copying.md for legal info.

#include "heuristics.h"

#include <algorithm>
#include <cmath>


namespace openage {
namespace path::legacy {

cost_old_t manhattan_cost(const coord::phys3 &start, const coord::phys3 &end) {
	cost_old_t dx = std::abs(start.ne - end.ne).to_float();
	cost_old_t dy = std::abs(start.se - end.se).to_float();
	return dx + dy;
}

cost_old_t chebyshev_cost(const coord::phys3 &start, const coord::phys3 &end) {
	cost_old_t dx = std::abs(start.ne - end.ne).to_float();
	cost_old_t dy = std::abs(start.se - end.se).to_float();
	return std::max(dx, dy);
}

cost_old_t euclidean_cost(const coord::phys3 &start, const coord::phys3 &end) {
	return (end - start).length();
}

cost_old_t euclidean_squared_cost(const coord::phys3 &start, const coord::phys3 &end) {
	cost_old_t dx = (start.ne - end.ne).to_float();
	cost_old_t dy = (start.se - end.se).to_float();
	return dx * dx + dy * dy;
}

} // namespace path::legacy
} // namespace openage
