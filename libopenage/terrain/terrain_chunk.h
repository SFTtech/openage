// Copyright 2013-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <json/json.h>
#include <stddef.h>
#include <vector>

#include "terrain.h"
#include "terrain_object.h"
#include "../coord/camgame.h"
#include "../coord/tile.h"
#include "../texture.h"
#include "../util/file.h"

namespace openage {

class Terrain;
class TerrainChunk;
class TileContent;
class TerrainObject;


/**
the number of tiles per direction on a chunk
*/
constexpr size_t chunk_size = 16;

/**
adjacent neighbors of a chunk.

neighbor ids:
      0
    7   1
  6   @   2
    5   3
      4
*/
struct chunk_neighbors {
	TerrainChunk *neighbor[8];
};

/**
terrain chunk class represents one chunk of the the drawn terrain.
*/
class TerrainChunk {
public:
	TerrainChunk();
	~TerrainChunk();

	/**
	 * stores the length for one chunk side.
	 */
	size_t size;

	/**
	 * number of tiles on that chunk (this->size^2)
	*/
	size_t tile_count;

	/**
	 * stores the chunk data, one tile_content struct for each tile.
	 */
	TileContent *data;

	/**
	 * the terrain to which this chunk belongs to.
	 */
	Terrain *terrain;

	/**
	 * the 8 neighbors this chunk has.
	 */
	chunk_neighbors neighbors;

	/**
	 * draws the terrain chunk on screen.
	 *
	 * @param chunk_pos the chunk position where it will be drawn
	 */
	void draw(coord::chunk chunk_pos);

	/**
	 * get tile data by coordinates.
	 */
	TileContent *get_data(coord::tile pos);

	/**
	 * get tile data by memory position.
	 */
	TileContent *get_data(size_t pos);

	/**
	 * get the tile data a given tile position relative to this chunk.
	 *
	 * also queries neighbors if the position is not on this chunk.
	 */
	TileContent *get_data_neigh(coord::tile pos);

	/**
	 * for savegame
	 */
	Json::Value toJson();

	int neighbor_id_by_pos(coord::tile pos);

	size_t tile_position(coord::tile pos);
	size_t tile_position_neigh(coord::tile pos);
	size_t get_tile_count();

	size_t tiles_in_row(unsigned int row);
	size_t get_size();

	void set_terrain(Terrain *parent);

	bool manually_created;

};

} // namespace openage
