#include "path.h"

#include <cmath>

namespace openage {
namespace path {

Node::Node()
	:
	accessible{false},
	visited{false},
	was_best{false},
	path_predecessor{nullptr} {
}

Node::~Node() {
}

bool Node::operator <(const Node &other) const {
	return this->future_cost < other.future_cost;
}

bool Node::operator ==(const Node &other) const {
	return this->position == other.position;
}

cost_t Node::cost_to(const Node &other) const {
	cost_t dx = this->position.ne - other.position.ne;
	cost_t dy = this->position.se - other.position.se;
	return std::hypot(dx, dy) * other.factor * this->factor;
}

Path Node::generate_backtrace() const {
	std::list<Node> waypoints;

	const Node *current = this;
	do {
		waypoints.push_front(*current);
		current = current->path_predecessor;
	} while (current != nullptr);

	return Path{waypoints};
}

std::vector<Node> Node::get_neighbors() const {
	return std::vector<Node>{};
}


Path::Path() {

}

Path::Path(const std::list<Node> &nodes)
	:
	waypoints{nodes} {
}

Path::~Path() {
}

} // namespace path
} // namespace openage
