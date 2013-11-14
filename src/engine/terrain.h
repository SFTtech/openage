#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <stddef.h>

#include "texture.h"
#include "coordinates.h"

namespace openage {
namespace engine {

extern coord::camera_delta tile_halfsize;

/**
terrain class represents the drawn terrain.
*/
class Terrain {
public:
	bool blending_enabled;

	Terrain(unsigned int height, size_t maxtextures, size_t maxblendmodes, int *priority_list);
	~Terrain();

	void draw();

	void set_tile(coord::tileno pos, int tile);
	int  get_tile(coord::tileno pos);


	size_t tile_position_diag(unsigned int row, unsigned int offset);
	size_t tile_position(coord::tileno pos);
	size_t get_tile_count();

	void set_texture(size_t index, engine::Texture *t);
	engine::Texture *get_texture(size_t index);
	size_t tiles_in_row(unsigned int row);
	size_t get_size();
	void set_mask(unsigned int modeid, engine::Texture *m);
	void set_blending(bool enabled);

private:
	size_t size;
	int *tiles;
	size_t tile_count;
	size_t num_rows;

	size_t texture_count, blendmode_count;
	engine::Texture **textures;
	engine::Texture **blendmasks;

	int *terrain_id_priority_map;

	unsigned get_subtexture_id(coord::tileno pos, unsigned atlas_size);
};

} //namespace engine
} //namespace openage

#endif //_TERRAIN_H_
