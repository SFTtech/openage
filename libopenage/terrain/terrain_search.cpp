// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include <cmath>

#include "terrain.h"
#include "terrain_object.h"
#include "terrain_search.h"

namespace openage {

TerrainObject *find_near(const TerrainObject &start,
                         std::function<bool(const TerrainObject &)> found,
                         unsigned int search_limit) {

	auto terrain = start.get_terrain();
	auto tile = start.pos.draw.to_tile3().to_tile();
	TerrainSearch search(terrain, tile);

	for (unsigned int i = 0; i < search_limit; ++i) {
		for (auto o : terrain->get_data(tile)->obj) {

			// invalid pointers are removed when the object is deleted
			if (found(*o)) {
				return o;
			}
		}
		tile = search.next_tile();
	}

	return nullptr;
}

TerrainObject *find_in_radius(const TerrainObject &start,
                              std::function<bool(const TerrainObject &)> found,
                              float radius) {
	auto terrain = start.get_terrain();
	coord::tile start_tile = start.pos.draw.to_tile3().to_tile();
	TerrainSearch search(terrain, start_tile, radius);

	// next_tile will first return the starting tile, so we need to run it once. We also
	// shouldn't discard this tile - if it isn't useful, ignore it in found
	coord::tile tile = search.next_tile();
	do {
		for (auto o : terrain->get_data(tile)->obj) {
			if (found(*o)) {
				return o;
			}
		}
		tile = search.next_tile();
		// coord::tile doesn't have a != operator, so we need to use !(a==b)
	} while (!(tile == start_tile));

	return nullptr;
}

TerrainSearch::TerrainSearch(std::shared_ptr<Terrain> t, coord::tile s)
	:
	TerrainSearch(t, s, .0f) {
}

TerrainSearch::TerrainSearch(std::shared_ptr<Terrain> t, coord::tile s, float radius)
	:
	terrain{t},
	start(s),
	previous_radius{.0f},
	max_radius{radius} {
}

coord::tile TerrainSearch::start_tile() const {
	return this->start;
}

void TerrainSearch::reset() {
	std::queue<coord::tile> empty;
	std::swap(this->tiles, empty);
	this->visited.clear();
	this->tiles.push(this->start);
	this->visited.insert(this->start);
}

coord::tile TerrainSearch::next_tile() {
	// conditions for returning to the initial tile
	if (this->tiles.empty() ||
	    (this->max_radius && this->previous_radius > this->max_radius)) {
		this->reset();
	}
	coord::tile result = this->tiles.front();
	this->tiles.pop();

	for (auto i = 0; i < 4; ++i) {
		auto to_add = result + neigh_tile[i];

		// check not visited and tile is within map
		if (this->visited.count(to_add) == 0 && terrain->get_data(to_add)) {
			this->visited.insert(to_add);
			this->tiles.push(to_add);
		}
	}

	this->previous_radius = std::hypot(result.ne - start.ne, result.se - start.se);
	return result;
}

} // namespace openage
