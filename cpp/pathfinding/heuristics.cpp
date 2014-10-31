// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#include "heuristics.h"

#include <cmath>


namespace openage {
namespace path {

cost_t manhattan_cost(const Node &start, const Node &end) {
	cost_t dx = std::abs((cost_t)start.position.ne - end.position.ne);
	cost_t dy = std::abs((cost_t)start.position.se - end.position.se);
	return dx + dy;
}

cost_t chebyshev_cost(const Node &start, const Node &end) {
	cost_t dx = start.position.ne - end.position.ne;
	cost_t dy = start.position.se - end.position.se;
	return std::max(dx, dy);
}

cost_t euclidean_cost(const Node &start, const Node &end) {
	cost_t dx = start.position.ne - end.position.ne;
	cost_t dy = start.position.se - end.position.se;
	return std::hypot(dx, dy);
}

} // namespace path
} // namespace openage
