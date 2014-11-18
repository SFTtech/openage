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
#include <map>
#include <unordered_set>

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
	auto valid_end = [&](const coord::phys3 &point) -> bool { return point == end; };
	auto h = [&](const coord::phys3 &point) -> cost_t { return euclidean_cost(point, end); };
	return a_star(start, valid_end, h, passable);
}

Path to_object(openage::TerrainObject *to_move,
               openage::TerrainObject *end) {
	coord::phys3 start = to_move->pos.draw;
	auto valid_end = [&](const coord::phys3 &pos) -> bool { return end->from_edge(pos) < to_move->min_axis() / 2; };
	auto heuristic = [&](const coord::phys3 &pos) -> cost_t { return std::max((double)(end->from_edge(pos) - to_move->min_axis() / 2), 0.0); };
	return a_star(start, valid_end, heuristic, to_move->passable);
}

Path find_nearest(coord::phys3 start,
                  std::function<bool(const coord::phys3 &)> valid_end,
                  std::function<bool(const coord::phys3 &)> passable) {
	// Use Dijkstra (hueristic = 0)
	auto zero = [](const coord::phys3 &) -> cost_t { return .0f; };
	return a_star(start, valid_end, zero, passable);
}

Path a_star(coord::phys3 start,
            std::function<bool(const coord::phys3 &)> valid_end,
            std::function<cost_t(const coord::phys3 &)> heuristic,
            std::function<bool(const coord::phys3 &)> passable) {

	// PairingHeap pop function currently crashes -- using priority queue for now
	//datastructure::PairingHeap<Node *> node_candidates;
	std::priority_queue<node_pt, std::vector<node_pt>, compare_node_cost> node_candidates;

	// list of known tiles and corresponding node.
	nodemap_t visited_tiles;

	// add starting node
	node_pt startNode = std::make_shared<Node>(start, nullptr, .0f, heuristic(start));
	visited_tiles[startNode->position] = startNode;
	node_candidates.push(startNode);

	// track the closest we can get to the end position
	// used when no path is found
	node_pt closest_node = startNode;

	// while the open list is not empty
	while (not node_candidates.empty()) {
		node_pt best_candidate = node_candidates.top();
		node_candidates.pop();
		best_candidate->was_best = true;

		if ( best_candidate->heuristic_cost < (1 << 13) || valid_end(best_candidate->position) ) {
			log::dbg("path cost is %f", best_candidate->future_cost);
			return best_candidate->generate_backtrace();
		}

		// closest node for cases when target cannot be reached
		if (best_candidate->heuristic_cost < closest_node->heuristic_cost) {
			closest_node = best_candidate;
		}

		for (node_pt neighbor : best_candidate->get_neighbors(visited_tiles)) {
			if (neighbor->was_best) {
				continue;
			}
			if ( !passable_line(best_candidate, neighbor, passable) ) {
				continue;
			}
			cost_t new_past_cost = best_candidate->past_cost + best_candidate->cost_to(*neighbor);

			// if new cost is better than the previous path
			bool not_visited = (visited_tiles.count(neighbor->position) == 0);
			if (not_visited or new_past_cost < neighbor->past_cost) {
				if (not_visited) {
					// calculate heuristic only once per node
					neighbor->heuristic_cost = heuristic(neighbor->position);
				}
				if (neighbor->heuristic_cost > closest_node->heuristic_cost * 3) {
					continue; // dont search forever...
				}

				// update new cost knowledge
				neighbor->past_cost = new_past_cost;
				neighbor->future_cost = neighbor->past_cost + neighbor->heuristic_cost;
				neighbor->path_predecessor = best_candidate;

				if (not_visited) {
					node_candidates.push(neighbor);
					visited_tiles[neighbor->position] = neighbor;
				} else {
					// just ignore until PairingHeap works
					//node_candidates.update(&neighbor);
				}
			}
		}
	}

	log::dbg("incomplete path cost is %f", closest_node->future_cost);
	return closest_node->generate_backtrace();
}

} // namespace path
} // namespace openage
