// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_TERRAIN_TERRAIN_SEARCH_H_
#define OPENAGE_TERRAIN_TERRAIN_SEARCH_H_

#include <functional>
#include <memory>
#include <queue>
#include <unordered_set>
#include <vector>

#include "../coord/tile.h"

namespace openage {

class Terrain;
class TerrainObject;

std::shared_ptr<TerrainObject> find_near(const TerrainObject &start,
                                         std::function<bool(const TerrainObject &)> found,
                                         unsigned int search_limit=500);

constexpr coord::tile_delta const neigh_tile[] = {
	{0,  1},
	{0, -1},
	{1,  0},
	{-1, 0}
};

/**
 * searches outward from a point and returns nearby objects
 * The state of the search is kept within the class, which allows
 * a user to look at a limited number of tiles per update cycle
 */
class TerrainSearch {
public:
	/**
	 * next_tile will cover all tiles on the map
	 */
	TerrainSearch(std::shared_ptr<Terrain> t, coord::tile s);

	/**
	 * next_tile will iterate over a range of tiles within a radius
	 */
	TerrainSearch(std::shared_ptr<Terrain> t, coord::tile s, float radius);
	~TerrainSearch() = default;

	/**
	 * the tile the search began on
	 */
	coord::tile start_tile() const;

	/**
	 * restarts the search from the start tile
	 */
	void reset();

	/**
	 * returns all objects on the next tile
	 */
	coord::tile next_tile();

private:
	const std::shared_ptr<Terrain> terrain;
	const coord::tile start;
	std::queue<coord::tile> tiles;
	std::unordered_set<coord::tile> visited;
	float previous_radius, max_radius;

};

} // namespace openage

#endif
