#include "heuristics.h"

#include <cmath>


namespace openage {
namespace path {

cost_t manhattan_cost(const Node &start, const Node &end) {
	cost_t dx = std::abs((cost_t)start.position.ne - end.position.ne);
	cost_t dy = std::abs((cost_t)start.position.se - end.position.se);
	return dx + dy;
}

cost_t costFunction(const Node& start, const Node& end, bool chebyshev,
		bool euclidean) {
	cost_t dx = start.position.ne - end.position.ne;
	cost_t dy = start.position.se - end.position.se;
	if (chebyshev)
		return std::max(dx, dy);

	if (euclidean)
		return std::hypot(dx, dy);
}

cost_t chebyshev_cost(const Node &start, const Node &end) {
	costFunction(start, end, true, false);
}

cost_t euclidean_cost(const Node &start, const Node &end) {
	costFunction(start, end, false, true);
}

} // namespace path
} // namespace openage
