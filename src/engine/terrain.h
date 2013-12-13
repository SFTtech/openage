#ifndef _ENGINE_TERRAIN_H_
#define _ENGINE_TERRAIN_H_

#include <stddef.h>

#include "texture.h"
#include "coord/camgame.h"
#include "coord/tile.h"
#include "util/file.h"

namespace engine {

/**
half the size of one terrain tile, in camgame
*/
extern coord::camgame_delta tile_halfsize;

/**
terrain class represents the drawn terrain.
*/
class Terrain {
public:
	bool blending_enabled;

	Terrain(unsigned int height, size_t maxtextures, size_t maxblendmodes, int *priority_list);
	~Terrain();

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

private:
	size_t size;
	int *tiles;
	size_t tile_count;
	size_t num_rows;

	size_t texture_count, blendmode_count;
	engine::Texture **textures;
	engine::Texture **blendmasks;

	int *terrain_id_priority_map;

	unsigned get_subtexture_id(coord::tile pos, unsigned atlas_size);
};

struct terrain_meta_line : util::line_data {
	~terrain_meta_line() {};

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

} //namespace engine

#endif //_ENGINE_TERRAIN_H_
