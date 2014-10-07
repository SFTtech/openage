#ifndef _ENGINE_TERRAIN_OBJECT_H_
#define _ENGINE_TERRAIN_OBJECT_H_

#include <stddef.h>

#include "terrain.h"
#include "terrain_chunk.h"
#include "texture.h"
#include "coord/tile.h"
#include "coord/phys3.h"

namespace openage {

class Terrain;
class TerrainChunk;


enum class object_state {
	placed,
	removed,
	floating,
};


/**
terrain object class represents one immobile object on the map (building, trees, fish, ...).
*/
class TerrainObject {
public:
	TerrainObject(Texture *tex, coord::tile_delta foundation_size, unsigned player);
	~TerrainObject();

	coord::tile start_pos;
	coord::phys3 draw_pos;
	coord::tile end_pos;

	/**
	 * tests whether this terrain object will fit at the given position.
	 *
	 * @param terrain: the terrain where the check will be performed.
	 * @param pos: the base position.
	 * @returns true when the object fits, false otherwise.
	 */
	bool fits(Terrain *terrain, coord::tile pos);

	/**
	 * binds the TerrainObject to a certain TerrainChunk.
	 *
	 * @param terrain: the terrain where the object will be placed onto.
	 * @param pos: (tile) position of the (nw,sw) corner
	 * @returns true when the object was placed, false when it did not fit at pos.
	 */
	bool place(Terrain *terrain, coord::tile pos);

	/**
	 * sets all the ground below the object to a terrain id.
	 *
	 * @param id: the terrain id to which the ground is set
	 * @param additional: amount of additional space arround the building
	 */
	void set_ground(int id, int additional=0);

	/**
	 * display the texture of this object at the placement position.
	 */
	bool draw();

	/**
	 * remove this TerrainObject from the terrain chunks.
	 */
	void remove();

	/**
	 * comparison for TerrainObjects.
	 *
	 * sorting for vertical placement.
	 * by using this order algorithm, the overlapping order
	 * is optimal so the objects can be drawn in correct order.
	 */
	bool operator <(const TerrainObject &other);


private:
	bool placed;
	Terrain *terrain;
	Texture *texture;
	coord::tile_delta size;
	unsigned player;

	int occupied_chunk_count;
	TerrainChunk *occupied_chunk[4];

	/**
	 * set and calculate object start and end positions.
	 *
	 * @param pos: the center position of the building
	 *
	 * set the center position to "middle",
	 * start_pos is % and end_pos = &
	 *
	 * for a building, the # tile will be "the clicked one":
	 *           @              @           @
	 *         @   @          @   @      %#   &
	 *       @   @   @      %   #   &       @
	 *     %   #   @   &      @   @
	 *       @   @   @          @
	 *         @   @
	 *           @
	 */
	void set_position(coord::tile pos);
};

} //namespace openage

#endif //_ENGINE_TERRAIN_OBJECT_H_
