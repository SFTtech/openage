// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#include "terrain_chunk.h"

#include <cmath>

#include "terrain_object.h"
#include "../engine.h"
#include "../log/log.h"
#include "../texture.h"
#include "../coord/tile.h"
#include "../coord/tile3.h"
#include "../coord/phys3.h"
#include "../coord/camgame.h"
#include "../util/error.h"
#include "../util/misc.h"

namespace openage {


TerrainChunk::TerrainChunk()
	:
	manually_created{true} {
	this->tile_count = std::pow(chunk_size, 2);

	// the data array for this chunk.
	// each element describes the tile data.
	this->data = new TileContent[this->tile_count];

	// initialize all neighbors as nonexistant
	for (int i = 0; i < 8; i++) {
		this->neighbors.neighbor[i] = nullptr;
	}

	log::log(MSG(dbg) << "Terrain chunk created: " <<
		"size=" << chunk_size << ", " <<
		"tiles=" << this->tile_count);
}


TerrainChunk::~TerrainChunk() {
	delete[] this->data;
}

TileContent *TerrainChunk::get_data(coord::tile pos) {
	return this->get_data(this->tile_position_neigh(pos));
}

TileContent *TerrainChunk::get_data(size_t pos) {
	return &this->data[pos];
}

TileContent *TerrainChunk::get_data_neigh(coord::tile pos) {
	// determine the neighbor id by the given position
	int neighbor_id = this->neighbor_id_by_pos(pos);

	// if the location is not on the current chunk, the neighbor id is != -1
	if (neighbor_id != -1) {

		// get the chunk where the requested neighbor tile lies on.
		TerrainChunk *neigh_chunk = this->neighbors.neighbor[neighbor_id];

		// this neighbor does not exist, so the tile does not exist.
		if (neigh_chunk == nullptr) {
			return nullptr;
		}

		// get position of tile on neighbor
		size_t pos_on_neighbor = this->tile_position_neigh(pos);

		return neigh_chunk->get_data(pos_on_neighbor);
	}
	// the position lies on the current chunk.
	else {
		return this->get_data(pos);
	}
}

/*
 * get the chunk neighbor id by a given position not lying on this chunk.
 *
 * neighbor ids:
 *
 *    ne
 * --
 * /|
 *       0
 *     7   1
 *   6   @   2
 *     5   3
 *       4
 * \|
 * --
 *   se
 */
int TerrainChunk::neighbor_id_by_pos(coord::tile pos) {
	int neigh_id = -1;

	if (pos.ne < 0) {
		if (pos.se < 0) {
			neigh_id = 6;
		}
		else if (pos.se >= (ssize_t)chunk_size) {
			neigh_id = 4;
		}
		else {
			neigh_id = 5;
		}
	}
	else if (pos.ne >= (ssize_t)chunk_size) {
		if (pos.se < 0) {
			neigh_id = 0;
		}
		else if (pos.se >= (ssize_t)chunk_size) {
			neigh_id = 2;
		}
		else {
			neigh_id = 1;
		}
	}
	else {
		if (pos.se < 0) {
			neigh_id = 7;
		}
		else if (pos.se >= (ssize_t)chunk_size) {
			neigh_id = 3;
		}
		else {
			neigh_id = -1;
		}
	}
	return neigh_id;
}

/*
 * calculates the memory position of a given tile location.
 *
 * give this function isometric coordinates, it returns the tile index.
 *
 * # is a single terrain tile:
 *
 *           3
 *         2   #
 *       1   #   #
 * ne= 0   #   *   #
 *       #   #   #   #
 * se= 0   #   #   #
 *       1   #   #
 *         2   #
 *           3
 *
 * for example, * is at position (2, 1)
 * the returned index would be 6 (count for each ne row, starting at se=0)
 */
size_t TerrainChunk::tile_position(coord::tile pos) {
	if (this->neighbor_id_by_pos(pos) != -1) {
		throw util::Error(MSG(err) << "Tile "
			"(" << pos.ne << ", " << pos.se << ") "
			"has been requested, but is not part of this chunk.");
	}

	return pos.se * chunk_size + pos.ne;
}

size_t TerrainChunk::tile_position_neigh(coord::tile pos) {
	// get position of tile on neighbor
	pos.ne = util::mod<coord::tile_t, chunk_size>(pos.ne);
	pos.se = util::mod<coord::tile_t, chunk_size>(pos.se);

	return pos.se * chunk_size + pos.ne;
}

size_t TerrainChunk::get_tile_count() {
	return this->tile_count;
}

size_t TerrainChunk::get_size() {
	return chunk_size;
}

void TerrainChunk::set_terrain(Terrain *parent) {
	this->terrain = parent;
}

} // namespace openage
