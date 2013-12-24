#ifndef _ENGINE_TERRAIN_H_
#define _ENGINE_TERRAIN_H_

#include <stddef.h>
#include <map>

#include "terrain_chunk.h"
#include "texture.h"
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

	Terrain(size_t terrain_meta_count, terrain_type *terrain_meta, size_t blending_meta_count, blending_mode *blending_meta);
	~Terrain();

	void attach_chunk(TerrainChunk *new_chunk, coord::chunk position);
	TerrainChunk *get_chunk(coord::chunk position);
	struct chunk_neighbors get_chunk_neighbors(coord::chunk position);
	void draw();

	unsigned get_subtexture_id(coord::tile pos, unsigned atlas_size);

	bool valid_terrain(size_t terrain_id);
	bool valid_mask(size_t mask_id);

	int priority(size_t terrain_id);
	int blendmode(size_t terrain_id);
	Texture *texture(size_t terrain_id);
	Texture *blending_mask(size_t mask_id);

	int get_blending_mode(size_t base_id, size_t neighbor_id);

	size_t terrain_type_count;
	size_t blendmode_count;

private:
	std::map<coord::chunk, TerrainChunk *, coord_chunk_compare> chunks;

	Texture **textures;
	Texture **blending_masks;

	int *terrain_id_priority_map;
	int *terrain_id_blendmode_map;

};

} //namespace engine

#endif //_ENGINE_TERRAIN_H_
