#ifndef _ENGINE_TERRAIN_H_
#define _ENGINE_TERRAIN_H_

#include <stddef.h>
#include <map>

#include "terrain_chunk.h"
#include "terrain_object.h"
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

/**
comparisons for chunk coordinates.

this allows storage of chunk coords as keys in a map.
*/
struct coord_chunk_compare {
	bool operator()(coord::chunk a, coord::chunk b) {
		return a.ne > b.ne || (a.ne == b.ne && a.se > b.se);
	}
};


/**
describes the properties of one terrain tile.

this includes the terrain_id (ice, water, grass, ...)
and the building standing on that tile.
*/
class TileContent {
public:
	TileContent();
	~TileContent();
	int terrain_id;
	TerrainObject *obj;
};


/**
describes the state of a terrain tile.
*/
enum tile_state {
	creatable,  //!< tile does not exist but can be created
	invalid,    //!< tile does not exist and can not be created
	existing,   //!< tile is already existing
};


/**
the terrain class is the main top-management interface for dealing with cost-benefit analysis to maximize company profits.

actually this is just the entrypoint and container for the terrain chunks.
*/
class Terrain {
public:
	Terrain(size_t terrain_meta_count, terrain_type *terrain_meta, size_t blending_meta_count, blending_mode *blending_meta, bool is_infinite);
	~Terrain();

	bool blending_enabled; //!< is terrain blending active. increases memory accesses by factor ~8
	bool infinite; //!< chunks are automagically created as soon as they are referenced

	coord::tile limit_positive, limit_negative; //!< for non-infinite terrains, this is the size limit.
	//TODO: non-square shaped terrain bounds

	/**
	fill the terrain with given terrain_id values.

	@returns whether the data filled on the terrain was cut because of
	         the terrains size limit.
	*/
	bool fill(const int *data, coord::tile_delta size);

	void attach_chunk(TerrainChunk *new_chunk, coord::chunk position, bool manual=true);
	TerrainChunk *get_chunk(coord::chunk position);
	TerrainChunk *get_chunk(coord::tile position);
	TerrainChunk *get_create_chunk(coord::chunk position);
	TerrainChunk *get_create_chunk(coord::tile position);

	TileContent *get_data(coord::tile position);

	struct chunk_neighbors get_chunk_neighbors(coord::chunk position);

	void draw();

	unsigned get_subtexture_id(coord::tile pos, unsigned atlas_size, coord::chunk chunk_pos={0,0});

	/**
	checks the creation state and premissions of a given tile position.
	*/
	tile_state check_tile(coord::tile position);

	/**
	checks whether the given tile position is allowed to exist on this terrain.
	*/
	bool check_tile_position(coord::tile position);

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
	/**
	maps chunk coordinates to chunks.
	*/
	std::map<coord::chunk, TerrainChunk *, coord_chunk_compare> chunks;

	Texture **textures;
	Texture **blending_masks;

	int *terrain_id_priority_map;
	int *terrain_id_blendmode_map;
};

} //namespace engine

#endif //_ENGINE_TERRAIN_H_
