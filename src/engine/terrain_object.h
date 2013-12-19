#ifndef _ENGINE_TERRAIN_OBJECT_H_
#define _ENGINE_TERRAIN_OBJECT_H_

#include <stddef.h>

#include "terrain_chunk.h"

namespace engine {

struct object_size {
	unsigned int se_length;
	unsigned int ne_length;
};

//fak u, dunno why i need u
class TerrainChunk;

/**
terrain chunk class represents one chunk of the the drawn terrain.
*/
class TerrainObject {
public:
	TerrainObject();
	~TerrainObject();
	bool bind_on_chunk(TerrainChunk *main_chunk, coord::tile pos);
	//init (terrain auf dreck setzen, richtige chunk->objects auf diesen pointer setzen, textur laden)

private:
	//tex (+draw starting point)
	Texture *texture;
	struct object_size size;

	int occupied_chunk_count;
	TerrainChunk *occupied_chunk[4];
};

} //namespace engine

#endif //_ENGINE_TERRAIN_OBJECT_H_
