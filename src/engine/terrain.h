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


/**
describes one terrrain type, like water, ice, etc.
*/
struct terrain_type {
	unsigned int id;
	int terrain_id;
	int slp_id;
	int sound_id;
	int blend_mode;
	int blend_priority;
	int angle_count;
	int frame_count;
	int terrain_dimensions0;
	int terrain_dimensions1;
	int terrain_replacement_id;

	int fill(const char *by_line);
};

/**
describes one blending mode, a blending transition shape between two different terrain types.
*/
struct blending_mode {
	unsigned int id;
	int mode_id;

	int fill(const char *by_line);
};

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
	struct chunk_neighbors get_chunk_neighbors(coord::chunk position);
	void draw();

private:
	std::map<coord::chunk, TerrainChunk *, coord_chunk_compare> chunks;
};

} //namespace engine

#endif //_ENGINE_TERRAIN_H_
