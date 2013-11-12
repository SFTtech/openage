#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <stddef.h>

#include "texture.h"

namespace openage {
namespace engine {

/**
terrain class represents the drawn terrain.
*/
class Terrain {
public:
	Terrain(unsigned int height, unsigned int maxtextures);
	~Terrain();

	void render();
	void set_tile_at(int index, int x, int y);
	void set_tile_at_row(int index, int row, int offset);
	void set_tile_at(int index, int position);
	int  tile_at(int x, int y);
	int  tile_at_row(int row, int offset);
	size_t tile_position_diag(unsigned int row, unsigned int offset);
	size_t tile_position(unsigned int x, unsigned int y);
	size_t get_tile_count();
	void set_texture(unsigned int index, engine::Texture *t);
	size_t tiles_in_row(unsigned int row);

private:
	size_t size;
	int *tiles;
	size_t tile_count;
	size_t num_rows;
	engine::Texture **textures;

	int get_subtexture_id(unsigned int x, unsigned int y);

};

} //namespace engine
} //namespace openage

#endif //_TERRAIN_H_
