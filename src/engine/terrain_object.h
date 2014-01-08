#ifndef _ENGINE_TERRAIN_OBJECT_H_
#define _ENGINE_TERRAIN_OBJECT_H_

#include <stddef.h>

#include "terrain_chunk.h"
#include "coord/tile.h"

namespace engine {

struct object_size {
	unsigned int se_length;
	unsigned int ne_length;
};

//fak u, dunno why i need u
class TerrainChunk;

/**
terrain object class represents one immobile object on the map (building, trees, ...).
*/
class TerrainObject {
public:
	TerrainObject(Texture *tex, unsigned player);
	TerrainObject(unsigned player);
	~TerrainObject();
	bool bind_on_chunk(TerrainChunk *main_chunk, coord::tile pos);
	bool draw();
	//init (terrain auf dreck setzen, richtige chunk->objects auf diesen pointer setzen, textur laden)

private:
	//tex (+draw starting point)
	Texture *texture;
	coord::tile pos;
	struct object_size size;
	unsigned player;

	int occupied_chunk_count;
	TerrainChunk *occupied_chunk[4];
};

} //namespace engine

#endif //_ENGINE_TERRAIN_OBJECT_H_
