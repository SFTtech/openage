#ifndef _ENGINE_TERRAIN_CHUNK_H_
#define _ENGINE_TERRAIN_CHUNK_H_

#include <stddef.h>

#include "terrain.h"
#include "texture.h"
#include "coord/camgame.h"
#include "coord/tile.h"
#include "util/file.h"

namespace engine {

class Terrain;
class TerrainChunk;
struct terrain_type;
struct blending_mode;

/**
adjacent neighbors of a chunk.
*/
struct chunk_neighbors {
	TerrainChunk *neighbor[8];
};

/**
terrain chunk class represents one chunk of the the drawn terrain.
*/
class TerrainChunk {
public:
	TerrainChunk(unsigned int size);
	~TerrainChunk();

	void draw();

	void set_tile(coord::tile pos, int tile);
	int  get_tile(coord::tile pos);

	size_t tile_position_diag(unsigned int row, unsigned int offset);
	size_t tile_position(coord::tile pos);
	size_t get_tile_count();

	size_t tiles_in_row(unsigned int row);
	size_t get_size();

	void set_terrain(Terrain *parent);

	// infinite terrain functionality
	/* chunk neighbor ids:
	      0
	    7   1
	  6   @   2
	    5   3
	      4
	*/
	chunk_neighbors neighbors;
	Terrain *terrain;

private:
	size_t size;
	int *tiles;
	size_t tile_count;
	size_t num_rows;
};

} //namespace engine

#endif //_ENGINE_TERRAIN_CHUNK_H_
