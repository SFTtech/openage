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

#include "path.h"
#include "../datastructure/pairing_heap.h"
#include "heuristics.h"


namespace openage {
namespace path {

Path a_star(Node start, Node end, heuristic_t heuristic) {
	datastructure::PairingHeap<Node&> node_candidates;

	start.past_cost = 0;
	start.future_cost = start.past_cost + heuristic(start, end);
	node_candidates.push(start);

	start.visited = true;

	// while the open list is not empty
	while (not node_candidates.empty()) {
		Node best_candidate = node_candidates.top();
		best_candidate.was_best = true;

		if (best_candidate == end) {
			return end.generate_backtrace();
		}

		for (Node &neighbor : best_candidate.get_neighbors()) {
			if (neighbor.was_best) {
				continue;
			}

			cost_t new_past_cost = best_candidate.past_cost + best_candidate.cost_to(neighbor);

			// if new cost is better than the previous path
			if (not neighbor.visited or new_past_cost < neighbor.past_cost) {

				if (not neighbor.visited) {
					// calculate heuristic only once per node
					neighbor.heuristic_cost = heuristic(neighbor, end);
				}

				// update new cost knowledge
				neighbor.past_cost = new_past_cost;
				neighbor.future_cost = neighbor.past_cost + neighbor.heuristic_cost;
				neighbor.path_predecessor = &best_candidate;

				if (not neighbor.visited) {
					node_candidates.push(neighbor);
					neighbor.visited = true;
				} else {
					node_candidates.update(neighbor);
				}
			}
		}
	}

	throw util::Error{"no path could be found"};
}

} // namespace path
} // namespace openage
