// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <memory>
#include <stddef.h>
#include <set>
#include <unordered_map>
#include <vector>

#include "../assetmanager.h"
#include "../texture.h"
#include "../coord/camgame.h"
#include "../coord/chunk.h"
#include "../util/misc.h"

namespace openage {

class Engine;
class RenderOptions;
class TerrainChunk;
class TerrainObject;

/**
 * type that for terrain ids.
 * it's signed so that -1 can indicate a missing tile.
 * TODO: get rid of the signedness.
 */
using terrain_t = int;

/**
 * hashing for chunk coordinates.
 *
 * this allows storage of chunk coords as keys in an unordered map.
 */
struct coord_chunk_hash {
	size_t operator ()(const coord::chunk &input) const {
		constexpr int half_size_t_bits = sizeof(size_t) * 4;

		return ((size_t)input.ne << half_size_t_bits) | input.se;
	}
};


/**
 * describes the properties of one terrain tile.
 *
 * this includes the terrain_id (ice, water, grass, ...)
 * and the list of objects which have a bounding box overlapping the tile
 */
class TileContent {
public:
	TileContent();
	~TileContent();
	terrain_t terrain_id;
	std::vector<TerrainObject *> obj;
};


/**
 * coordinate offsets for getting tile neighbors by their id.
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
 * describes the state of a terrain tile.
 */
enum class tile_state {
	missing,    //!< tile is not created yet
	existing,   //!< tile is already existing
	creatable,  //!< tile does not exist but can be created
	invalid,    //!< tile does not exist and can not be created
};


/**
 * storage for influences by neighbor tiles.
 */
struct influence {
	uint8_t direction;    //!< bitmask for influence directions, bit 0 = neighbor 0, etc.
	int priority;         //!< the blending priority for this influence
	terrain_t terrain_id; //!< the terrain id of the influence
};

/**
 * influences for one tile.
 * as a tile has 8 adjacent and diagonal neighbors,
 * the maximum number of influences is 8.
 */
struct influence_group {
	int count;
	terrain_t terrain_ids[8];
	struct influence data[8];
};

/**
 * one influence on another tile.
 */
struct neighbor_tile {
	terrain_t terrain_id;
	tile_state state;
	int priority;
};

/**
 * storage data for a single terrain tile.
 */
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

/**
 * collection of drawing data for a single tile.
 * because of influences, a maximum of 8+1 draws
 * could be requested.
 */
struct tile_draw_data {
	ssize_t count;
	struct tile_data data[9];
};

/**
 * the complete render instruction collection for the terrain.
 * this is passed to the renderer and will be drawn on screen.
 */
struct terrain_render_data {
	std::vector<struct tile_draw_data> tiles;
	std::set<TerrainObject *, util::less<TerrainObject *>> objects;
};

/**
 * specification for all available
 * tile types and blending data
 */
struct terrain_meta {
	size_t terrain_id_count;
	size_t blendmode_count;

	std::vector<Texture *> textures;
	std::vector<Texture *> blending_masks;

	std::unique_ptr<int[]> terrain_id_priority_map;
	std::unique_ptr<int[]> terrain_id_blendmode_map;

	std::unique_ptr<uint8_t[]> terrain_id_map_color_hi_map;
	std::unique_ptr<uint8_t[]> terrain_id_map_color_med_map;
	std::unique_ptr<uint8_t[]> terrain_id_map_color_low_map;

	std::unique_ptr<influence[]> influences_buf;
};

/**
 * the terrain class is the main top-management interface
 * for dealing with cost-benefit analysis to maximize company profits.
 *
 * actually this is just the entrypoint and container for the terrain chunks.
 */
class Terrain {
public:
	Terrain(terrain_meta *meta, bool is_infinite);
	~Terrain();

	bool infinite; //!< chunks are automagically created as soon as they are referenced

	coord::tile limit_positive, limit_negative; //!< for non-infinite terrains, this is the size limit.
	//TODO: non-square shaped terrain bounds

	/**
	 * returns a list of all referenced chunks
	 */
	std::vector<coord::chunk> used_chunks() const;

	/**
	 * fill the terrain with given terrain_id values.
	 * @returns whether the data filled on the terrain was cut because of
	 * the terrains size limit.
	 */
	bool fill(const int *data, coord::tile_delta size);

	/**
	 * Attach a chunk to the terrain, to a given position.
	 *
	 * @param new_chunk The chunk to be attached
	 * @param position The chunk position where the chunk will be placed
	 * @param manually_created Was this chunk created manually? If true, it will not be free'd automatically
	 */
	void attach_chunk(TerrainChunk *new_chunk, coord::chunk position, bool manual=true);

	/**
	 * get a terrain chunk by a given chunk position.
	 *
	 * @return the chunk if exists, nullptr else
	 */
	TerrainChunk *get_chunk(coord::chunk position);

	/**
	 * get a terrain chunk by a given tile position.
	 *
	 * @return the chunk it exists, nullptr else
	 */
	TerrainChunk *get_chunk(coord::tile position);

	/**
	 * get or create a terrain chunk for a given chunk position.
	 *
	 * @return the (maybe newly created) chunk
	 */
	TerrainChunk *get_create_chunk(coord::chunk position);

	/**
	 * get or create a terrain chunk for a given tile position.
	 *
	 * @return the (maybe newly created) chunk
	 */
	TerrainChunk *get_create_chunk(coord::tile position);

	/**
	 * return tile data for the given position.
	 *
	 * the only reason the chunks exist, is because of this data.
	 */
	TileContent *get_data(coord::tile position);

	/**
	 * an object which contains the given point, null otherwise
	 */
	TerrainObject *obj_at_point(const coord::phys3 &point);

	/**
	 * get the neighbor chunks of a given chunk.
	 *
	 *
	 * chunk neighbor ids:
	 *       0   / <- ne
	 *     7   1
	 *   6   @   2
	 *     5   3
	 *       4   \ <- se
	 *
	 *     ne se
	 *     0:  1 -1
	 *     1:  1  0
	 *     2:  1  1
	 *     3:  0  1
	 *     4: -1  1
	 *     5: -1  0
	 *     6: -1 -1
	 *     7:  0 -1
	 *
	 * @param position: the position of the center chunk.
	 */
	struct chunk_neighbors get_chunk_neighbors(coord::chunk position);

	/**
	 * return the subtexture offset id for a given tile position.
	 * the maximum offset is determined by the atlas size.
	 *
	 * this function returns always the right value, so that neighbor tiles
	 * of the same terrain (like grass-grass) are matching (without blendomatic).
	 * -> e.g. grass only map.
	 */
	unsigned get_subtexture_id(coord::tile pos, unsigned atlas_size);

	/**
	 * checks the creation state and premissions of a given tile position.
	 */
	tile_state check_tile(coord::tile position);

	/**
	 * checks whether the given tile position is allowed to exist on this terrain.
	 */
	bool check_tile_position(coord::tile position);

	/**
	 * validate whether the given terrain id is available.
	 */
	bool validate_terrain(terrain_t terrain_id);

	/**
	 * validate whether the given mask id is available.
	 */
	bool validate_mask(ssize_t mask_id);

	/**
	 * return the blending priority for a given terrain id.
	 */
	int priority(terrain_t terrain_id);

	/**
	 * return the blending mode/blendomatic mask set for a given terrain id.
	 */
	int blendmode(terrain_t terrain_id);

	/**
	* get the red color component for a given terrain id.
	*/
	uint8_t map_color_hi(terrain_t terrain_id);

	/**
	* get the red color component for a given terrain id.
	*/
	uint8_t map_color_med(terrain_t terrain_id);

	/**
	* get the red color component for a given terrain id.
	*/
	uint8_t map_color_low(terrain_t terrain_id);

	/**
	 * get the terrain texture for a given terrain id.
	 */
	Texture *texture(terrain_t terrain_id);

	/**
	 * get the blendomatic mask with the given mask id.
	 */
	Texture *blending_mask(ssize_t mask_id);

	/**
	 * return the blending mode id for two given neighbor ids.
	 */
	int get_blending_mode(terrain_t base_id, terrain_t neighbor_id);

	/**
	 * draw the currently visible terrain area on screen.
	 * @param engine: the engine where the terrain should be drawn to.
	 */
	void draw(Engine *engine, RenderOptions *settings);

	/**
	 * create the drawing instruction data.
	 *
	 * created draw data according to the given tile boundaries.
	 *
	 *
	 * @param ab: upper left tile
	 * @param cd: upper right tile
	 * @param ef: lower right tile
	 * @param gh: lower left tile
	 *
	 * @returns a drawing instruction struct that contains all information for rendering
	 */
	struct terrain_render_data create_draw_advice(coord::tile ab, coord::tile cd,
	                                              coord::tile ef, coord::tile gh,
	                                              bool blending_enabled);

	/**
	 * create rendering and blending information for a single tile on the terrain.
	 */
	struct tile_draw_data create_tile_advice(coord::tile position, bool blending_enabled);

	/**
	 * gather neighbors of a given base tile.
	 *
	 * @param basepos: the base position, around which the neighbors will be fetched
	 * @param neigh_tiles: the destination buffer where the neighbors will be stored
	 * @param influences_by_terrain_id: influence buffer that is reset in the same step
	 */
	void get_neighbors(coord::tile basepos,
	                   struct neighbor_tile *neigh_tiles,
	                   struct influence *influences_by_terrain_id);

	/**
	 * look at neighbor tiles around the base_tile, and store the influence bits.
	 *
	 * @param base_tile: the base tile for which influences are calculated
	 * @param neigh_tiles: the neigbors of base_tile
	 * @param influences_by_terrain_id: influences will be stored to this buffer, as bitmasks
	 * @returns an influence group that describes the maximum 8 possible influences on the base_tile
	 */
	struct influence_group calculate_influences(struct tile_data *base_tile,
	                                            struct neighbor_tile *neigh_tiles,
	                                            struct influence *influences_by_terrain_id);

	/**
	 * calculate blending masks for a given tile position.
	 *
	 * @param position: the base tile position, for which the masks are calculated
	 * @param tile_data: the buffer where the created drawing layers will be stored in
	 * @param influences: the buffer where calculated influences were stored to
	 *
	 * @see calculate_influences
	 */
	void calculate_masks(coord::tile position,
	                     struct tile_draw_data *tile_data,
	                     struct influence_group *influences);

	/**
	 * @return min/max+1 ne/se coordinates among the referenced chunks
	 *
	 * This doesn't mean that chunks with these specific coordinates exist.
	 */
	std::tuple<coord::chunk, coord::chunk> used_bounding_rect() const;

private:

	/**
	 * terrain meta data
	 */
	terrain_meta *meta;

	/**
	 * maps chunk coordinates to chunks.
	 */
	std::unordered_map<coord::chunk, TerrainChunk *, coord_chunk_hash> chunks;
};

} // namespace openage
