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
	Terrain(unsigned int height);
	~Terrain();

	void render();
	void set_tile_at(int index, int row, int offset);
	void set_tile_at(int index, int position);
	int  tile_at(int row, int offset);
	size_t tile_position(unsigned int row, unsigned int offset);
	size_t get_tile_count();
	void set_texture(engine::Texture *t);
	size_t tiles_in_row(unsigned int row);

private:
	size_t size;
	int *tiles;
	size_t tile_count;
	size_t num_rows;
	engine::Texture *texture;

};

} //namespace engine
} //namespace openage

#endif //_TERRAIN_H_
