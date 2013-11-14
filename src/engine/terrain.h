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
	Terrain(unsigned int height, unsigned int maxtextures);
	~Terrain();

	void draw();

	void set_tile(coord::tileno pos, int tile);
	int  get_tile(coord::tileno pos);


	size_t tile_position_diag(unsigned int row, unsigned int offset);
	size_t tile_position(coord::tileno pos);
	size_t get_tile_count();
	void set_texture(unsigned int index, engine::Texture *t);
	size_t tiles_in_row(unsigned int row);
	size_t get_size();

private:
	size_t size;
	int *tiles;
	size_t tile_count;
	size_t num_rows;
	engine::Texture **textures;

	unsigned get_subtexture_id(coord::tileno pos, unsigned atlas_size);
};

} //namespace engine
} //namespace openage

#endif //_TERRAIN_H_
