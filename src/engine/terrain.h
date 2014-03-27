#ifndef _ENGINE_TERRAIN_H_
#define _ENGINE_TERRAIN_H_

#include <functional>
#include <stddef.h>
#include <set>
#include <unordered_map>
#include <vector>

#include "terrain_chunk.h"
#include "terrain_object.h"
#include "texture.h"
#include "coord/camgame.h"
#include "coord/chunk.h"
#include "util/misc.h"
#include "../gamedata/blending_mode.h"
#include "../gamedata/terrain.h"

namespace engine {

class TerrainChunk;

/**
type that for terrain ids.
*/
using terrain_t = int;

/**
half the size of one terrain diamond tile, in camgame
*/
extern coord::camgame_delta tile_halfsize;

/**
hashing for chunk coordinates.

this allows storage of chunk coords as keys in an unordered map.
*/
struct coord_chunk_hash {
	size_t operator()(const coord::chunk input) const {
		std::hash<coord::chunk_t> chunk_t_hasher{};
		size_t ne_hash = chunk_t_hasher(input.ne);
		size_t se_hash = chunk_t_hasher(input.se);
		return ne_hash ^ se_hash;
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
	terrain_t terrain_id;
	TerrainObject *obj;
};


/**
coordinate offsets for getting tile neighbors by their id.
*/
constexpr coord::tile_delta const neigh_offsets[] = {
	{ 1, -1},
	{ 1,  0},
	{ 1,  1},
	{ 0,  1},
	{-1,  1},
	{-1,  0},
	{-1, -1},
	{ 0, -1}
};


/**
describes the state of a terrain tile.
*/
enum class tile_state {
	missing,    //!< tile is not created yet
	existing,   //!< tile is already existing
	creatable,  //!< tile does not exist but can be created
	invalid,    //!< tile does not exist and can not be created
};


/**
storage for influences by neighbor tiles.
*/
struct influence {
	uint8_t direction; //!< bitmask for influence directions, bit 0 = neighbor 0, etc.
	int priority;   //!< the blending priority for this influence
	terrain_t terrain_id; //!< the terrain id of the influence
};

struct influence_group {
	int count;
	terrain_t terrain_ids[8];
	struct influence data[8];
};

struct neighbor_tile {
	terrain_t terrain_id;
	tile_state state;
	int priority;
};

struct tile_data {
	terrain_t terrain_id;
	coord::tile pos;
	int subtexture_id;
	Texture *tex;
	int priority;
	int mask_id;
	int blend_mode;
	Texture *mask_tex;
	tile_state state;
};

struct tile_draw_data {
	ssize_t count;
	struct tile_data data[9];
};

struct terrain_render_data {
	std::vector<struct tile_draw_data> tiles;
	std::set<TerrainObject *, util::less<TerrainObject *>> objects;
};

/**
the terrain class is the main top-management interface for dealing with cost-benefit analysis to maximize company profits.

actually this is just the entrypoint and container for the terrain chunks.
*/
class Terrain {
public:
	Terrain(std::vector<terrain_type> terrain_meta, std::vector<blending_mode> blending_meta, bool is_infinite);
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

	/**
	return tile data for the given position.

	the only reason the chunks exist, is because of this data.
	*/
	TileContent *get_data(coord::tile position);

	struct chunk_neighbors get_chunk_neighbors(coord::chunk position);

	unsigned get_subtexture_id(coord::tile pos, unsigned atlas_size);

	/**
	checks the creation state and premissions of a given tile position.
	*/
	tile_state check_tile(coord::tile position);

	/**
	checks whether the given tile position is allowed to exist on this terrain.
	*/
	bool check_tile_position(coord::tile position);

	bool validate_terrain(terrain_t terrain_id);
	bool validate_mask(ssize_t mask_id);

	int priority(terrain_t terrain_id);
	int blendmode(terrain_t terrain_id);
	Texture *texture(terrain_t terrain_id);
	Texture *blending_mask(ssize_t mask_id);

	/**
	return the blending mode id for two given neighbor ids.
	*/
	int get_blending_mode(terrain_t base_id, terrain_t neighbor_id);

	/**
	draw the currently visible terrain area on screen.
	*/
	void draw();

	/**
	create the drawing instruction data.

	created draw data according to the given tile boundaries.

	@param ab: upper left tile
	@param cd: upper right tile
	@param ef: lower right tile
	@param gh: lower left tile

	@returns a drawing instruction struct that contains all information for rendering
	*/
	struct terrain_render_data create_draw_advice(coord::tile ab, coord::tile cd, coord::tile ef, coord::tile gh);

	/**
	create rendering and blending information for a single tile on the terrain.
	*/
	struct tile_draw_data create_tile_advice(coord::tile position);

	/**
	gather neighbors of a given base tile.

	@param basepos: the base position, around which the neighbors will be fetched
	@param neigh_tiles: the destination buffer where the neighbors will be stored
	@param influences_by_terrain_id: influence buffer that is reset in the same step
	*/
	void get_neighbors(coord::tile basepos, struct neighbor_tile *neigh_tiles, struct influence *influences_by_terrain_id);

	/**
	look at neighbor tiles around the base_tile, and store the influence bits.

	@param base_tile: the base tile for which influences are calculated
	@param neigh_tiles: the neigbors of base_tile
	@param influences_by_terrain_id: influences will be stored to this buffer, as bitmasks
	@returns an influence group that describes the maximum 8 possible influences on the base_tile
	*/
	struct influence_group calculate_influences(struct tile_data *base_tile, struct neighbor_tile *neigh_tiles, struct influence *influences_by_terrain_id);

	/**
	calculate blending masks for a given tile position.

	@param position: the base tile position, for which the masks are calculated
	@param tile_data: the buffer where the created drawing layers will be stored in
	@param influences: the buffer where calculated influences were stored to

	@see calculate_influences
	*/
	void calculate_masks(coord::tile position, struct tile_draw_data *tile_data, struct influence_group *influences);

	size_t terrain_id_count;
	size_t blendmode_count;

private:
	/**
	maps chunk coordinates to chunks.
	*/
	std::unordered_map<coord::chunk, TerrainChunk *, coord_chunk_hash> chunks;

	Texture **textures;
	Texture **blending_masks;

	int *terrain_id_priority_map;
	int *terrain_id_blendmode_map;

	struct influence *influences_buf;
};

} //namespace engine

#endif //_ENGINE_TERRAIN_H_
