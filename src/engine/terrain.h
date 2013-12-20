#ifndef _ENGINE_TERRAIN_H_
#define _ENGINE_TERRAIN_H_

#include <stddef.h>
#include <map>

#include "terrain_chunk.h"
#include "coord/camgame.h"
#include "coord/chunk.h"

namespace engine {

class TerrainChunk;

/**
half the size of one terrain diamond tile, in camgame
*/
extern coord::camgame_delta tile_halfsize;



struct coord_chunk_compare {
	bool operator()(coord::chunk a, coord::chunk b) {
		//TODO: yeah, optimize that..
		return ((a.ne < b.ne) && (a.se < b.se));
	}
};


/**
terrain chunk class represents one chunk of the the drawn terrain.
*/
class Terrain {
public:
	bool blending_enabled;

	Terrain();
	~Terrain();

	void attach_chunk(TerrainChunk *new_chunk, coord::chunk position);
	TerrainChunk *get_chunk(coord::chunk position);

	void draw();

private:
	std::map<coord::chunk, TerrainChunk *, coord_chunk_compare> chunks;
};

} //namespace engine

#endif //_ENGINE_TERRAIN_H_
