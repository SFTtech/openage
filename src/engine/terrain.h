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
	bool blending_enabled;

	Terrain(unsigned int height, size_t maxtextures, size_t maxblendmodes);
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


	int get_subtexture_id(unsigned int x, unsigned int y);

};

} //namespace engine
} //namespace openage

#endif //_TERRAIN_H_
