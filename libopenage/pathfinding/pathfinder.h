// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <unordered_map>


namespace openage::path {
class Grid;
class Integrator;

/**
 * Pathfinder for flow field pathfinding.
 *
 * The pathfinder manages the grids defining the pathable ingame areas and
 * provides an interface for making pathfinding requests.
 *
 * Pathfinding consists of a multi-step process: First, there is a high-level
 * search using A* to identify the sectors of the grid that should be traversed.
 * Afterwards, flow fields are calculated from the target sector to the start
 * sector, which are then used to guide the actual unit movement.
 */
class Pathfinder {
public:
	/**
	 * Create a new pathfinder.
	 */
	Pathfinder();
	~Pathfinder() = default;

	/**
	 * Get the grid at a specified index.
	 *
	 * @param id ID of the grid.
	 *
	 * @return Pathfinding grid.
	 */
	const std::shared_ptr<Grid> &get_grid(size_t id) const;

private:
	/**
	 * Grids managed by this pathfinder.
	 *
	 * Each grid can have separate pathing.
	 */
	std::unordered_map<size_t, std::shared_ptr<Grid>> grids;

	/**
	 * Integrator for flow field calculations.
	 */
	std::shared_ptr<Integrator> integrator;
};

} // namespace openage::path
