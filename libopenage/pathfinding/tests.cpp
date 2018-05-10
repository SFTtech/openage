// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "../log/log.h"
#include "../testing/testing.h"

#include "heuristics.h"
#include "path.h"

namespace openage {
namespace path {
namespace tests {

/**
 * This function tests setting up basic nodes that point to a previous node.
 * Tests that direction is set correctly and that factor is set correctly.
 */
void node_0() {
	coord::phys3 p0{0, 0, 0};
	coord::phys3 p1{1, 0, 0};
	coord::phys3 p2{1, 1, 0};
	coord::phys3 p3{1, -1, 0};
	coord::phys3 p4{2, 0, 0};
	coord::phys3 p5{2, 2, 0};
	coord::phys3 p6{2, -2, 0};

	node_pt n0 = std::make_unique<Node>(p0, nullptr);
	node_pt n1 = std::make_unique<Node>(p1, n0);
	node_pt n2 = std::make_unique<Node>(p2, n1);
	node_pt n3 = std::make_unique<Node>(p3, n1);
	node_pt n4 = std::make_unique<Node>(p0, n1);

	// Testing how the factor is effected from the change in
	// direction from one node to another

	(n1->dir_ne == 1) or TESTFAIL;
	(n1->dir_se == 0) or TESTFAIL;

	// Expect this to be 2 since the similarity between nodes is zero
	(n1->factor == 2) or TESTFAIL;

	(n2->dir_ne == 0) or TESTFAIL;
	(n2->dir_se == 1) or TESTFAIL;

	// Expect this to be 2 since it takes a 90 degree turn from n1
	(n2->factor == 2) or TESTFAIL;

	(n3->dir_ne == 0) or TESTFAIL;
	(n3->dir_se == -1) or TESTFAIL;

	// Expect this to be 2 since it takes a 90 degree turn from n1
	(n3->factor == 2) or TESTFAIL;

	(n4->dir_ne == -1) or TESTFAIL;
	(n4->dir_se == 0) or TESTFAIL;

	// Expect this to be 3 since it takes a 180 degree turn from n1
	(n4->factor == 3) or TESTFAIL;

	// Testing that the distance from the previous node noes not
	// effect the factor, only change in direction

	n1 = std::make_unique<Node>(p4, n0);
	n2 = std::make_unique<Node>(p5, n1);
	n3 = std::make_unique<Node>(p6, n1);
	n4 = std::make_unique<Node>(p0, n1);

	(n1->dir_ne == 1) or TESTFAIL;
	(n1->dir_se == 0) or TESTFAIL;

	// Expect this to be 2 since the similarity between nodes is zero
	(n1->factor == 2) or TESTFAIL;
	(n2->dir_ne == 0) or TESTFAIL;
	(n2->dir_se == 1) or TESTFAIL;

	// Expect this to be 2 since it takes a 90 degree turn from n1
	(n2->factor == 2) or TESTFAIL;
	(n3->dir_ne == 0) or TESTFAIL;
	(n3->dir_se == -1) or TESTFAIL;

	// Expect this to be 2 since it takes a 90 degree turn from n1
	(n3->factor == 2) or TESTFAIL;
	(n4->dir_ne == -1) or TESTFAIL;
	(n4->dir_se == 0) or TESTFAIL;

	// Expect this to be 3 since it takes a 180 degree turn from n1
	(n4->factor == 3) or TESTFAIL;
}

/**
 * This function tests Node->cost_to. The testing is done on 2 unrelated
 * nodes (They have no previous node) to test the basic cost without adding
 * the cost from node->factor.
 */
void node_cost_to_0() {
	// Testing basic cost_to with ne only
	coord::phys3 p0{0, 0, 0};
	coord::phys3 p1{10, 0, 0};

	node_pt n0 = std::make_unique<Node>(p0, nullptr);
	node_pt n1 = std::make_unique<Node>(p1, nullptr);

	(n0->cost_to(*n1) == 10) or TESTFAIL;
	(n1->cost_to(*n0) == 10) or TESTFAIL;

	// Testing basic cost_to with se only
	coord::phys3 p2{0, 5, 0};

	node_pt n2 = std::make_unique<Node>(p2, nullptr);

	(n0->cost_to(*n2) == 5) or TESTFAIL;
	(n2->cost_to(*n0) == 5) or TESTFAIL;

	// Testing basic cost_to, up has no effect
	coord::phys3 p3{4, 3, 3};

	node_pt n3 = std::make_unique<Node>(p3, nullptr);

	(n0->cost_to(*n3) == 5) or TESTFAIL;
	(n3->cost_to(*n0) == 5) or TESTFAIL;
}

/**
 * This function tests Node->cost_to. The testing is done on the neighbor
 * nodes to test how the directional factor effects the cost.
 */
void node_cost_to_1() {
	// Set up coords so that n1 will have a direction of ne = 1
	// but n0 with not be in n1s neighbors
	coord::phys3 p0{-8192, 0, 0};
	coord::phys3 p1{8192, 0, 0};

	node_pt n0 = std::make_unique<Node>(p0, nullptr);
	node_pt n1 = std::make_unique<Node>(p1, n0);

	// We wxpect twice the normal cost since n0 had not direction
	// thus we get a factor of 2 on n1
	(n0->cost_to(*n1) == 32768) or TESTFAIL;
	(n1->cost_to(*n0) == 32768) or TESTFAIL;

	nodemap_t visited_tiles;
	visited_tiles[n0->position] = n0;
	std::vector<float> costs;

	// Collect the costs to go to all the neighbors of n1
	for (node_pt neighbor : n1->get_neighbors(visited_tiles, 1)) {
		costs.push_back(n1->cost_to(*neighbor));
	}

	cost_t cost = costs.back();
	costs.pop_back();
	(cost == 32768) or TESTFAIL;

	cost = costs.back();
	costs.pop_back();
	// TODO find a better way to check the float values
	(cost <= 62725) or TESTFAIL;
	(cost >= 62723) or TESTFAIL;

	cost = costs.back();
	costs.pop_back();
	(cost == 49152) or TESTFAIL;

	cost = costs.back();
	costs.pop_back();
	(cost <= 62725) or TESTFAIL;
	(cost >= 62723) or TESTFAIL;

	cost = costs.back();
	costs.pop_back();
	(cost == 32768) or TESTFAIL;

	cost = costs.back();
	costs.pop_back();
	(cost <= 29957) or TESTFAIL;
	(cost >= 29955) or TESTFAIL;

	cost = costs.back();
	costs.pop_back();
	// We expect this to be the cheapest since it is in the same
	// direction that n1 is going
	(cost == 16384) or TESTFAIL;

	cost = costs.back();
	costs.pop_back();
	(cost <= 29957) or TESTFAIL;
	(cost >= 29955) or TESTFAIL;
}

/**
 * This function does a basic test of generating a backtrace from the
 * last node in a path.
 */
void node_generate_backtrace_0() {
	coord::phys3 p0{0, 0, 0};
	coord::phys3 p1{10, 0, 0};
	coord::phys3 p2{20, 0, 0};
	coord::phys3 p3{30, 0, 0};

	node_pt n0 = std::make_unique<Node>(p0, nullptr);
	node_pt n1 = std::make_unique<Node>(p1, n0);
	node_pt n2 = std::make_unique<Node>(p2, n1);
	node_pt n3 = std::make_unique<Node>(p3, n2);

	Path path = n3->generate_backtrace();

	(path.waypoints[0] == *n3) or TESTFAIL;
	(path.waypoints[1] == *n2) or TESTFAIL;
	(path.waypoints[2] == *n1) or TESTFAIL;
}

/**
 * This function tests Node->get_neighbors and how the scale effects
 * the neighbors given.
 */
void node_get_neighbors_0() {
	coord::phys3 p0{0, 0, 0};

	node_pt n0 = std::make_unique<Node>(p0, nullptr);
	nodemap_t map;

	// Testing get_neighbors returning all surounding tiles with
	// a factor of 1

	std::vector<node_pt> neighbors = n0->get_neighbors(map, 1);

	(neighbors[0]->position.ne == 8192) or TESTFAIL;
	(neighbors[0]->position.se == -8192) or TESTFAIL;

	(neighbors[1]->position.ne == 8192) or TESTFAIL;
	(neighbors[1]->position.se == 0) or  TESTFAIL;

	(neighbors[2]->position.ne == 8192) or TESTFAIL;
	(neighbors[2]->position.se == 8192) or TESTFAIL;

	(neighbors[3]->position.ne == 0) or TESTFAIL;
	(neighbors[3]->position.se == 8192) or TESTFAIL;

	(neighbors[4]->position.ne == -8192) or TESTFAIL;
	(neighbors[4]->position.se == 8192) or TESTFAIL;

	(neighbors[5]->position.ne == -8192) or TESTFAIL;
	(neighbors[5]->position.se == 0) or TESTFAIL;

	(neighbors[6]->position.ne == -8192) or TESTFAIL;
	(neighbors[6]->position.se == -8192) or TESTFAIL;

	(neighbors[7]->position.ne == 0) or TESTFAIL;
	(neighbors[7]->position.se == -8192) or TESTFAIL;

	(neighbors.size() == 8) or TESTFAIL;

	// Testing how a larger scale changes the neighbors generated

	neighbors = n0->get_neighbors(map, 2);

	(neighbors[0]->position.ne == 16384) or TESTFAIL;
	(neighbors[0]->position.se == -16384) or TESTFAIL;

	(neighbors[1]->position.ne == 16384) or TESTFAIL;
	(neighbors[1]->position.se == 0) or TESTFAIL;

	(neighbors[2]->position.ne == 16384) or TESTFAIL;
	(neighbors[2]->position.se == 16384) or TESTFAIL;

	(neighbors[3]->position.ne == 0) or TESTFAIL;
	(neighbors[3]->position.se == 16384) or TESTFAIL;

	(neighbors[4]->position.ne == -16384) or TESTFAIL;
	(neighbors[4]->position.se == 16384) or TESTFAIL;

	(neighbors[5]->position.ne == -16384) or TESTFAIL;
	(neighbors[5]->position.se == 0) or TESTFAIL;

	(neighbors[6]->position.ne == -16384) or TESTFAIL;
	(neighbors[6]->position.se == -16384) or TESTFAIL;

	(neighbors[7]->position.ne == 0) or TESTFAIL;
	(neighbors[7]->position.se == -16384) or TESTFAIL;

	(neighbors.size() == 8) or TESTFAIL;
}

/**
 * This is a helper passable function that alwalys returns true.
 */
bool always_passable(const coord::phys3 &) {
	return true;
}

/**
 * This is a helper passable function that always returns false.
 */
bool not_passable(const coord::phys3 &) {
	return false;
}

/**
 * This is a helper passable function that only returns true when
 * pos.ne == 20.
 */
bool sometimes_passable(const coord::phys3 &pos) {
	if (pos.ne == 20) {
		return false;
	} else {
		return true;
	}
}

/**
 * This function tests passable_line. Tests with always false, always true,
 * and position dependant functions being passed in as args.
 */
void node_passable_line_0() {
	coord::phys3 p0{0, 0, 0};
	coord::phys3 p1{1000, 0, 0};

	node_pt n0 = std::make_unique<Node>(p0, nullptr);
	node_pt n1 = std::make_unique<Node>(p1, n0);

	(path::passable_line(n0, n1, path::tests::always_passable) == true) or TESTFAIL;
	(path::passable_line(n0, n1, path::tests::not_passable) == false) or TESTFAIL;

	// The next 2 cases show that a different sample can change the results
	// for the same path
	(path::passable_line(n0, n1, path::tests::sometimes_passable, 10) == true) or TESTFAIL;
	(path::passable_line(n0, n1, path::tests::sometimes_passable, 50) == false) or TESTFAIL;
}

/**
 * Top level node test.
 */
void path_node() {
	node_0();
	node_cost_to_0();
	node_cost_to_1();
	node_generate_backtrace_0();
	node_get_neighbors_0();
	node_passable_line_0();
}

} // namespace tests
} // namespace pathfinding
} // namespace openage
