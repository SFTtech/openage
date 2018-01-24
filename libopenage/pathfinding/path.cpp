// Copyright 2014-2018 the openage authors. See copying.md for legal info.

#include <cmath>

#include "path.h"
#include "../terrain/terrain.h"

namespace openage {
namespace path {


bool compare_node_cost::operator ()(const node_pt lhs, const node_pt rhs) const {
	// TODO: use node operator <
	return lhs->future_cost < rhs->future_cost;
}


Node::Node(const coord::phys3 &pos, node_pt prev)
	:
	position(pos),
	tile_position(pos.to_tile3().to_tile()),
	direction{},
	visited{false},
	was_best{false},
	factor{1.0f},
	path_predecessor{prev},
	heap_node(nullptr) {

	if (prev) {
		this->direction = (this->position - prev->position).normalize();

		// TODO: add dot product to coord
		cost_t similarity = ((this->direction.ne.to_float() *
		                      prev->direction.ne.to_float()) +
		                     (this->direction.se.to_float() *
		                      prev->direction.se.to_float()));
		this->factor += (1 - similarity);
	}
}


Node::Node(const coord::phys3 &pos, node_pt prev, cost_t past, cost_t heuristic)
	:
	Node{pos, prev} {
	this->past_cost = past;
	this->heuristic_cost = heuristic;
	this->future_cost = past + heuristic;
}


bool Node::operator <(const Node &other) const {
	return this->future_cost < other.future_cost;
}


bool Node::operator ==(const Node &other) const {
	return this->position == other.position;
}


cost_t Node::cost_to(const Node &other) const {
	// ignore the up-position, thus convert to phys2
	return ((this->position - other.position).to_phys2().length()
	        * other.factor * this->factor);
}


Path Node::generate_backtrace() {
	std::vector<Node> waypoints;

	node_pt current = this->shared_from_this();
	do {
		Node other = *current;
		waypoints.push_back(*current);
		current = current->path_predecessor;
	} while (current != nullptr);
	waypoints.pop_back(); // remove start

	return {waypoints};
}


std::vector<node_pt> Node::get_neighbors(const nodemap_t &nodes, float scale) {
	std::vector<node_pt> neighbors;
	neighbors.reserve(8);
	for (int n = 0; n < 8; ++n) {
		coord::phys3 n_pos = this->position + (neigh_phys[n] * scale);

		if (nodes.count(n_pos) > 0) {
			neighbors.push_back( nodes.at(n_pos) );
		}
		else {
			neighbors.push_back( std::make_shared<Node>(n_pos, this->shared_from_this()) );
		}
	}
	return neighbors;
}


bool passable_line(node_pt start, node_pt end, std::function<bool(const coord::phys3 &)> passable, float samples) {
	// interpolate between points and make passablity checks
	// (dont check starting position)
	for (int i = 1; i <= samples; ++i) {
		// TODO: needs more fixed-point
		double percent = (double) i / samples;
		coord::phys_t ne = (1.0 - percent) * start->position.ne.to_double() + percent * end->position.ne.to_double();
		coord::phys_t se = (1.0 - percent) * start->position.se.to_double() + percent * end->position.se.to_double();
		coord::phys_t up = (1.0 - percent) * start->position.up.to_double() + percent * end->position.up.to_double();

		if (!passable(coord::phys3{ne, se, up})) {
			return false;
		}
	}
	return true;
}


Path::Path(const std::vector<Node> &nodes)
	:
	waypoints{nodes} {}


void Path::draw_path(const coord::CoordManager &mgr) {
	glLineWidth(1);
	glColor3f(0.3, 1.0, 0.3);
	glBegin(GL_LINES); {
		for (Node &n : waypoints) {
			coord::viewport draw_pos = n.position.to_viewport(mgr);
			glVertex3f(draw_pos.x, draw_pos.y, 0);
		}
	}
	glEnd();
}


}} // openage::path
