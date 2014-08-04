#ifndef _ENGINE_TERRAIN_OBJECT_H_
#define _ENGINE_TERRAIN_OBJECT_H_

#include <stddef.h>

#include "terrain.h"
#include "terrain_chunk.h"
#include "texture.h"
#include "coord/tile.h"
#include "coord/phys3.h"

namespace openage {

struct object_size {
	unsigned int ne_length;
	unsigned int se_length;
};

//fak u, dunno why i need u
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
	TerrainObject(Texture *tex, unsigned player);
	~TerrainObject();

	coord::tile start_pos;
	coord::phys3 draw_pos;
	coord::tile end_pos;

	bool fits(Terrain *terrain, coord::tile pos);
	bool place(Terrain *terrain, coord::tile pos);
	void set_ground(int id, int additional=0);
	bool draw();
	void remove();


	/**
	comparison for TerrainObjects.

	sorting for vertical placement,
	so the objects can be drawn in correct order.
	*/
	bool operator <(const TerrainObject &other);


private:
	bool placed;
	Terrain *terrain;
	Texture *texture;
	struct object_size size;
	unsigned player;

	int occupied_chunk_count;
	TerrainChunk *occupied_chunk[4];

	void set_position(coord::tile pos);
};

} //namespace openage

#endif //_ENGINE_TERRAIN_OBJECT_H_
