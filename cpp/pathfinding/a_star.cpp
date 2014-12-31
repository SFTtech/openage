// Copyright 2014-2014 the openage authors. See copying.md for legal info.

/** @file
 *
 * This file implements the A* search algorithm for openage.
 *
 * Literature:
 * Hart, Peter E., Nils J. Nilsson, and Bertram Raphael. "A formal basis for
 * the heuristic determination of minimum cost paths."  Systems Science and
 * Cybernetics, IEEE Transactions on 4, no. 2 (1968): 100-107.
 */

#include "a_star.h"

#include <cmath>

#include "../datastructure/pairing_heap.h"
#include "../log.h"
#include "../terrain/terrain.h"
#include "path.h"
#include "heuristics.h"


namespace openage {
namespace path {

Path to_point(coord::phys3 start,
              coord::phys3 end,
              std::function<bool(const coord::phys3 &)> passable) {
	auto valid_end = [&](const coord::phys3 &point) -> bool {
		coord::phys_t dx = point.ne - end.ne;
		coord::phys_t dy = point.se - end.se;
		return std::hypot(dx, dy) < path_grid_size;
	};

	auto heuristic = [&](const coord::phys3 &point){
		return euclidean_cost(point, end);
	};
	return a_star(start, valid_end, heuristic, passable);
}

Path to_object(openage::TerrainObject *to_move,
               openage::TerrainObject *end) {
	coord::phys3 start = to_move->pos.draw;
	auto valid_end = [&](const coord::phys3 &pos) {
		return end->from_edge(pos) < (path_grid_size + to_move->min_axis() / 2);
	};
	auto heuristic = [&](const coord::phys3 &pos) {
		return end->from_edge(pos) - to_move->min_axis() / 2;
	};
	return a_star(start, valid_end, heuristic, to_move->passable);
}

Path find_nearest(coord::phys3 start,
                  std::function<bool(const coord::phys3 &)> valid_end,
                  std::function<bool(const coord::phys3 &)> passable) {
	// Use Dijkstra (hueristic = 0)
	auto zero = [](const coord::phys3 &) { return .0f; };
	return a_star(start, valid_end, zero, passable);
}

Path a_star(coord::phys3 start,
            std::function<bool(const coord::phys3 &)> valid_end,
            std::function<cost_t(const coord::phys3 &)> heuristic,
            std::function<bool(const coord::phys3 &)> passable) {

	// improved allocator for nodes - similar performance
	// to stack and takes care of deallocation
	util::stack_allocator<Node> alloc(1000);

	//temporary storage for neighbors
	node_pt neighbors[8];

	// path node storage, always provides cheapest next node.
	heap_t node_candidates;

	// list of known tiles and corresponding node.
	nodemap_t visited_tiles;

	// add starting node
	node_pt start_node = alloc.create(start, nullptr, .0f, heuristic(start));
	visited_tiles[start_node->position] = start_node;
	node_candidates.push(start_node);

	start_node->heap_node = node_candidates.push(start_node);

	// track the closest we can get to the end position
	// used when no path is found
	node_pt closest_node = start_node;

	// while there are candidates to visit
	while (not node_candidates.empty()) {
		node_pt best_candidate = node_candidates.pop();

		best_candidate->was_best = true;

		// node to terminate the search was found
		if (valid_end(best_candidate->position)) {
			log::dbg("path cost is %f", best_candidate->future_cost);
			log::dbg("Total nodes created: %d", visited_tiles.size());
			auto rval = closest_node->generate_backtrace();
			log::dbg("Number of nodes in path: %d", rval.waypoints.size());
			return rval;
		}

		// closest node for cases when target cannot be reached
		if (best_candidate->heuristic_cost < closest_node->heuristic_cost) {
			closest_node = best_candidate;
		}

		// evaluate all neighbors of the current candidate for further progress
		best_candidate->get_neighbors(visited_tiles, neighbors, alloc);
		for (node_pt neighbor : neighbors) {
			if (neighbor->was_best) {
				continue;
			}
			if (not passable_line(best_candidate, neighbor, passable)) {
				continue;
			}

			bool not_visited = (visited_tiles.count(neighbor->position) == 0);
			cost_t new_past_cost = best_candidate->past_cost +best_candidate->cost_to(*neighbor);

			// if new cost is better than the previous path
			if (not_visited or new_past_cost < neighbor->past_cost) {
				if (not_visited) {
					// calculate heuristic only once per node
					neighbor->heuristic_cost = heuristic(neighbor->position);
				}
				if (neighbor->heuristic_cost > closest_node->heuristic_cost * 3) {
					continue; // dont search forever...
				}

				// update new cost knowledge
				neighbor->past_cost        = new_past_cost;
				neighbor->future_cost      = neighbor->past_cost + neighbor->heuristic_cost;
				neighbor->path_predecessor = best_candidate;

				if (not_visited) {
					neighbor->heap_node = node_candidates.push(neighbor);
					visited_tiles[neighbor->position] = neighbor;
				} else {
					node_candidates.update(neighbor->heap_node);
				}
			}
		}
	}

	log::dbg("incomplete path cost is %f", closest_node->future_cost);
	log::dbg("Total nodes created: %d", visited_tiles.size());
	
	auto rval = closest_node->generate_backtrace();
	log::dbg("Number of nodes in path: %d", rval.waypoints.size());
	return rval;
}

} // namespace path
} // namespace openage
