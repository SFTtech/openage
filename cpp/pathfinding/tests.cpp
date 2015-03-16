// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "../log.h"
#include "../util/error.h"
#include "heuristics.h"
#include "path.h"

namespace openage {
namespace path {
namespace tests {

/**
 * This function tests setting up basic nodes that point to a previous node.
 * Tests that direction is set correctly and that factor is set correctly.
 */
int node_0() {
	int stage = 0;

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

	stage += 1; // 1
	if (not (n1->dir_ne == 1)) { return stage; }
	if (not (n1->dir_se == 0)) { return stage; }

	stage += 1; // 2
	// Expect this to be 2 since the similarity between nodes is zero
	if (not (n1->factor == 2)) { return stage; }

	stage += 1; // 3
	if (not (n2->dir_ne == 0)) { return stage; }
	if (not (n2->dir_se == 1)) { return stage; }

	stage += 1; // 4
	// Expect this to be 2 since it takes a 90 degree turn from n1
	if (not (n2->factor == 2)) { return stage; }

	stage += 1; // 5
	if (not (n3->dir_ne == 0)) { return stage; }
	if (not (n3->dir_se == -1)) { return stage; }

	stage += 1; // 6
	// Expect this to be 2 since it takes a 90 degree turn from n1
	if (not (n3->factor == 2)) { return stage; }

	stage += 1; // 7
	if (not (n4->dir_ne == -1)) { return stage; }
	if (not (n4->dir_se == 0)) { return stage; }

	stage += 1; // 8
	// Expect this to be 3 since it takes a 180 degree turn from n1
	if (not (n4->factor == 3)) { return stage; }

	// Testing that the distance from the previous node noes not
	// effect the factor, only change in direction

	stage += 1; // 9
	n1 = std::make_unique<Node>(p4, n0);
	n2 = std::make_unique<Node>(p5, n1);
	n3 = std::make_unique<Node>(p6, n1);
	n4 = std::make_unique<Node>(p0, n1);

	stage += 1; // 10
	if (not (n1->dir_ne == 1)) { return stage; }
	if (not (n1->dir_se == 0)) { return stage; }

	stage += 1; // 11
	// Expect this to be 2 since the similarity between nodes is zero
	if (not (n1->factor == 2)) { return stage; }

	stage += 1; // 12
	if (not (n2->dir_ne == 0)) { return stage; }
	if (not (n2->dir_se == 1)) { return stage; }

	stage += 1; // 13
	// Expect this to be 2 since it takes a 90 degree turn from n1
	if (not (n2->factor == 2)) { return stage; }

	stage += 1; // 14
	if (not (n3->dir_ne == 0)) { return stage; }
	if (not (n3->dir_se == -1)) { return stage; }

	stage += 1; // 15
	// Expect this to be 2 since it takes a 90 degree turn from n1
	if (not (n3->factor == 2)) { return stage; }

	stage += 1; // 16
	if (not (n4->dir_ne == -1)) { return stage; }
	if (not (n4->dir_se == 0)) { return stage; }

	stage += 1; // 17
	// Expect this to be 3 since it takes a 180 degree turn from n1
	if (not (n4->factor == 3)) { return stage; }

	return -1;
}

/**
 * This function tests Node->cost_to. The testing is done on 2 unrelated
 * nodes (They have no previous node) to test the basic cost without adding
 * the cost from node->factor.
 */
int node_cost_to_0() {
	int stage = 0;

	// Testing basic cost_to with ne only
	coord::phys3 p0{0, 0, 0};
	coord::phys3 p1{10, 0, 0};

	node_pt n0 = std::make_unique<Node>(p0, nullptr);
	node_pt n1 = std::make_unique<Node>(p1, nullptr);

	if (not (n0->cost_to(*n1) == 10)) { return stage; }
	if (not (n1->cost_to(*n0) == 10)) { return stage; }

	stage += 1; // 1

	// Testing basic cost_to with se only
	coord::phys3 p2{0, 5, 0};

	node_pt n2 = std::make_unique<Node>(p2, nullptr);

	if (not (n0->cost_to(*n2) == 5)) { return stage; }
	if (not (n2->cost_to(*n0) == 5)) { return stage; }

	stage += 1; // 2

	// Testing basic cost_to, up has no effect
	coord::phys3 p3{4, 3, 3};

	node_pt n3 = std::make_unique<Node>(p3, nullptr);

	if (not (n0->cost_to(*n3) == 5)) {return stage; }
	if (not (n3->cost_to(*n0) == 5)) {return stage; }

	return -1;
}

/**
 * This function tests Node->cost_to. The testing is done on the neighbor
 * nodes to test how the directional factor effects the cost.
 */
int node_cost_to_1() {
	int stage = 0;

	// Set up coords so that n1 will have a direction of ne = 1
	// but n0 with not be in n1s neighbors
	coord::phys3 p0{-8192, 0, 0};
	coord::phys3 p1{8192, 0, 0};

	node_pt n0 = std::make_unique<Node>(p0, nullptr);
	node_pt n1 = std::make_unique<Node>(p1, n0);

	// We wxpect twice the normal cost since n0 had not direction
	// thus we get a factor of 2 on n1
	if (not (n0->cost_to(*n1) == 32768)) { return stage; }
	if (not (n1->cost_to(*n0) == 32768)) { return stage; }

	stage += 1; // 1

	nodemap_t visited_tiles;
	visited_tiles[n0->position] = n0;
	std::vector<float> costs;

	// Collect the costs to go to all the neighbors of n1
	for (node_pt neighbor : n1->get_neighbors(visited_tiles, 1)) {
		costs.push_back(n1->cost_to(*neighbor));
	}

	cost_t cost = costs.back();
	costs.pop_back();
	if (not (cost == 32768)) {return stage; }

	stage += 1; // 2
	cost = costs.back();
	costs.pop_back();
	// TODO find a better way to check the float values
	if (not (cost <= 62725)) {return stage; }
	if (not (cost >= 62723)) {return stage; }

	stage += 1; // 3
	cost = costs.back();
	costs.pop_back();
	if (not (cost == 49152)) {return stage; }

	stage += 1; // 4
	cost = costs.back();
	costs.pop_back();
	if (not (cost <= 62725)) {return stage; }
	if (not (cost >= 62723)) {return stage; }

	stage += 1; // 5
	cost = costs.back();
	costs.pop_back();
	if (not (cost == 32768)) {return stage; }

	stage += 1; // 6
	cost = costs.back();
	costs.pop_back();
	if (not (cost <= 29957)) {return stage; }
	if (not (cost >= 29955)) {return stage; }

	stage += 1; // 7
	cost = costs.back();
	costs.pop_back();
	// We expect this to be the cheapest since it is in the same
	// direction that n1 is going
	if (not (cost == 16384)) {return stage; }

	stage += 1; // 8
	cost = costs.back();
	costs.pop_back();
	if (not (cost <= 29957)) {return stage; }
	if (not (cost >= 29955)) {return stage; }

	return -1;
}

/**
 * This function does a basic test of generating a backtrace from the
 * last node in a path.
 */
int node_generate_backtrace_0() {
	int stage = 0;

	coord::phys3 p0{0, 0, 0};
	coord::phys3 p1{10, 0, 0};
	coord::phys3 p2{20, 0, 0};
	coord::phys3 p3{30, 0, 0};

	node_pt n0 = std::make_unique<Node>(p0, nullptr);
	node_pt n1 = std::make_unique<Node>(p1, n0);
	node_pt n2 = std::make_unique<Node>(p2, n1);
	node_pt n3 = std::make_unique<Node>(p3, n2);

	stage += 1; // 1

	Path path = n3->generate_backtrace();

	if (not (path.waypoints[0] == *n3)) { return stage; }

	stage += 1; // 2
	if (not (path.waypoints[1] == *n2)) { return stage; }

	stage += 1; // 3
	if (not (path.waypoints[2] == *n1)) { return stage; }

	return -1;
}

/**
 * This function tests Node->get_neighbors and how the scale effects
 * the neighbors given.
 */
int node_get_neighbors_0() {
	int stage = 0;

	coord::phys3 p0{0, 0, 0};

	node_pt n0 = std::make_unique<Node>(p0, nullptr);
	nodemap_t map;

	// Testing get_neighbors returning all surounding tiles with
	// a factor of 1

	stage += 1; // 1
	std::vector<node_pt> neighbors = n0->get_neighbors(map, 1);

	stage += 1; // 2
	if (not (neighbors[0]->position.ne == 8192)) { return stage; }
	if (not (neighbors[0]->position.se == -8192)) { return stage; }

	stage += 1; // 3
	if (not (neighbors[1]->position.ne == 8192)) { return stage; }
	if (not (neighbors[1]->position.se == 0)) { return stage; }

	stage += 1; // 4
	if (not (neighbors[2]->position.ne == 8192)) { return stage; }
	if (not (neighbors[2]->position.se == 8192)) { return stage; }

	stage += 1; // 5
	if (not (neighbors[3]->position.ne == 0)) { return stage; }
	if (not (neighbors[3]->position.se == 8192)) { return stage; }

	stage += 1; // 6
	if (not (neighbors[4]->position.ne == -8192)) { return stage; }
	if (not (neighbors[4]->position.se == 8192)) { return stage; }

	stage += 1; // 7
	if (not (neighbors[5]->position.ne == -8192)) { return stage; }
	if (not (neighbors[5]->position.se == 0)) { return stage; }

	stage += 1; // 8
	if (not (neighbors[6]->position.ne == -8192)) { return stage; }
	if (not (neighbors[6]->position.se == -8192)) { return stage; }

	stage += 1; // 9
	if (not (neighbors[7]->position.ne == 0)) { return stage; }
	if (not (neighbors[7]->position.se == -8192)) { return stage; }

	stage += 1; // 10
	if (not (neighbors.size() == 8)) { return stage; }

	// Testing how a larger scale changes the neighbors generated

	stage += 1; // 11
	neighbors = n0->get_neighbors(map, 2);

	stage += 1; // 12
	if (not (neighbors[0]->position.ne == 16384)) { return stage; }
	if (not (neighbors[0]->position.se == -16384)) { return stage; }

	stage += 1; // 13
	if (not (neighbors[1]->position.ne == 16384)) { return stage; }
	if (not (neighbors[1]->position.se == 0)) { return stage; }

	stage += 1; // 14
	if (not (neighbors[2]->position.ne == 16384)) { return stage; }
	if (not (neighbors[2]->position.se == 16384)) { return stage; }

	stage += 1; // 15
	if (not (neighbors[3]->position.ne == 0)) { return stage; }
	if (not (neighbors[3]->position.se == 16384)) { return stage; }

	stage += 1; // 16
	if (not (neighbors[4]->position.ne == -16384)) { return stage; }
	if (not (neighbors[4]->position.se == 16384)) { return stage; }

	stage += 1; // 17
	if (not (neighbors[5]->position.ne == -16384)) { return stage; }
	if (not (neighbors[5]->position.se == 0)) { return stage; }

	stage += 1; // 18
	if (not (neighbors[6]->position.ne == -16384)) { return stage; }
	if (not (neighbors[6]->position.se == -16384)) { return stage; }

	stage += 1; // 19
	if (not (neighbors[7]->position.ne == 0)) { return stage; }
	if (not (neighbors[7]->position.se == -16384)) { return stage; }

	stage += 1; // 20
	if (not (neighbors.size() == 8)) { return stage; }

	return -1;
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
int node_passable_line_0() {
	int stage = 0;

	coord::phys3 p0{0, 0, 0};
	coord::phys3 p1{1000, 0, 0};

	node_pt n0 = std::make_unique<Node>(p0, nullptr);
	node_pt n1 = std::make_unique<Node>(p1, n0);

	stage += 1; // 1

	bool pass = path::passable_line(n0, n1, path::tests::always_passable);
	if (not (pass == true)) { return stage; }

	stage += 1; // 2

	pass = path::passable_line(n0, n1, path::tests::not_passable);
	if (not (pass == false)) { return stage; }

	stage += 1; // 3

	// The next 2 cases show that a different sample can change the results
	// for the same path
	pass = path::passable_line(n0, n1, path::tests::sometimes_passable, 10);
	if (not (pass == true)) { return stage; }

	stage += 1; // 4

	pass = path::passable_line(n0, n1, path::tests::sometimes_passable, 50);
	if (not (pass == false)) { return stage; }

	return -1;
}

/**
 * Top level node test.
 */
void path_node() {
	int ret;
	const char *testname;

	if ((ret = node_0()) != -1) {
		testname = "Simple node test";
		goto out;
	}

	if ((ret = node_cost_to_0()) != -1) {
		testname = "Simple node convert_to test";
		goto out;
	}

	if ((ret = node_cost_to_1()) != -1) {
		testname = "Node convert_to with factor test";
		goto out;
	}

	if ((ret = node_generate_backtrace_0()) != -1) {
		testname = "Node generate_backtrace test";
		goto out;
	}

	if ((ret = node_get_neighbors_0()) != -1) {
		testname = "Node get_neighbors test";
		goto out;
	}

	if ((ret = node_passable_line_0()) != -1) {
		testname = "Node passable_line test";
		goto out;
	}

	return;

out:
	log::err("%s failed at stage %d", testname, ret);
	throw "failed node tests";
}

} // namespace tests
} // namespace pathfinding
} // namespace openage
