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
struct terrain_type;
struct blending_mode;

/**
terrain chunk class represents one chunk of the the drawn terrain.
*/
class TerrainChunk {
public:
	TerrainChunk(unsigned int size, size_t terrain_meta_count, terrain_type *terrain_meta, size_t blending_meta_count, blending_mode *blending_meta);
	~TerrainChunk();

	void draw();

	void set_tile(coord::tile pos, int tile);
	int  get_tile(coord::tile pos);

	size_t tile_position_diag(unsigned int row, unsigned int offset);
	size_t tile_position(coord::tile pos);
	size_t get_tile_count();

	void set_texture(size_t index, engine::Texture *t);
	engine::Texture *get_texture(size_t index);
	size_t tiles_in_row(unsigned int row);
	size_t get_size();
	void set_mask(unsigned int modeid, engine::Texture *m);
	void set_blending(bool enabled);

	int get_blending_mode(size_t base_id, size_t neighbor_id);

	void set_terrain(Terrain *parent);

private:
	size_t size;
	int *tiles;
	size_t tile_count;
	size_t num_rows;

	size_t terrain_type_count;
	engine::Texture **textures;

	size_t blendmode_count;
	engine::Texture **blending_masks;

	int *terrain_id_priority_map;
	int *terrain_id_blendmode_map;

	unsigned get_subtexture_id(coord::tile pos, unsigned atlas_size);

	// infinite terrain functionality
	/* chunk neighbor ids:
	      0
	    7   1
	  6   @   2
	    5   3
	      4
	*/
	TerrainChunk *neighbor[8];
	Terrain *terrain;

};

} //namespace engine

#endif //_ENGINE_TERRAIN_CHUNK_H_
