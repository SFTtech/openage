#ifndef _ENGINE_TERRAIN_CHUNK_H_
#define _ENGINE_TERRAIN_CHUNK_H_

#include <stddef.h>
#include <vector>

#include "terrain.h"
#include "texture.h"
#include "terrain_object.h"
#include "coord/camgame.h"
#include "coord/tile.h"
#include "util/file.h"

namespace engine {

class Terrain;
class TerrainChunk;
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

constexpr coord::tile_delta const neigh_offsets[] = {
	{ 1, -1},
	{ 1,  0},
	{ 1,  1},
	{ 0,  1},
	{-1,  1},
	{-1,  0},
	{-1, -1},
	{ 0, -1}
};


/**
terrain chunk class represents one chunk of the the drawn terrain.
*/
class TerrainChunk {
public:
	TerrainChunk();
	~TerrainChunk();

	void draw(coord::chunk chunk_pos);

	void set_tile(coord::tile pos, int tile);
	void set_tile(size_t pos, int tile);
	int  get_tile(coord::tile pos);
	int  get_tile(size_t pos);
	void set_object(coord::tile pos, TerrainObject *obj);
	TerrainObject *get_object(coord::tile pos);

	int  get_tile_neigh(coord::tile pos);
	int  neighbor_id_by_pos(coord::tile pos);

	size_t tile_position(coord::tile pos);
	size_t tile_position_neigh(coord::tile pos);
	size_t get_tile_count();

	size_t tiles_in_row(unsigned int row);
	size_t get_size();

	void set_terrain(Terrain *parent);

	// infinite terrain functionality
	chunk_neighbors neighbors;
	Terrain *terrain;
	bool manually_created;

	std::vector<TerrainObject *> object_list;
	TerrainObject **object;

private:
	size_t size;
	int *tiles;
	size_t tile_count;
};

} //namespace engine

#endif //_ENGINE_TERRAIN_CHUNK_H_
