#ifndef _ENGINE_TERRAIN_H_
#define _ENGINE_TERRAIN_H_

#include <stddef.h>
#include <map>

#include "terrain_chunk.h"
#include "coord/camgame.h"
#include "coord/chunk.h"

namespace engine {

/**
half the size of one terrain diamond tile, in camgame
*/
extern coord::camgame_delta tile_halfsize;


/**
terrain chunk class represents one chunk of the the drawn terrain.
*/
class Terrain {
public:
	Terrain();
	~Terrain();

private:
	std::map<coord::chunk, TerrainChunk *> chunk;
};

} //namespace engine

#endif //_ENGINE_TERRAIN_H_
