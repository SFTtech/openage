#include "terrain_object.h"

#include "engine.h"
#include "texture.h"
#include "log.h"
#include "terrain.h"
#include "terrain_chunk.h"
#include "util/error.h"
#include "coord/tile.h"
#include "coord/tile3.h"
#include "coord/phys3.h"
#include "coord/camgame.h"

namespace engine {

TerrainObject::TerrainObject(Texture *tex, unsigned player) {
	this->placed = false;
	this->texture = tex;
	//university specific for testing purposes
	this->size = { 5, 5 };
	this->player = player;

	this->occupied_chunk_count = 0;
}


TerrainObject::~TerrainObject() {}


/**
* remove this TerrainObject from the terrain chunks.
*/
void TerrainObject::remove() {
	if (this->occupied_chunk_count == 0) {
		return;
	}

	//remove from drawing list
	for (unsigned i = 0; i < this->occupied_chunk[0]->object_list.size(); i++) {
		if (this->occupied_chunk[0]->object_list[i] == this) {
			this->occupied_chunk[0]->object_list.erase(this->occupied_chunk[0]->object_list.begin()+i);
		}
	}

	//remove terrain_chunk->object values
	//TODO: brute force, should be faster...
	for (int i = 0; i < this->occupied_chunk_count; i++) {
		for (unsigned pos = 0; pos < this->occupied_chunk[i]->get_tile_count(); pos++) {
			if (this->occupied_chunk[i]->object[pos] == this) {
				this->occupied_chunk[i]->object[pos] = nullptr;
			}
		}
	}
}


/**
* binds the TerrainObject to a certain TerrainChunk.
*
* @param terrain: the terrain where the object will be placed onto.
* @param pos: (tile) position of the (nw,sw) corner
* @returns true when the object was placed, false when it did not fit at pos.
*/
bool TerrainObject::place(Terrain *terrain, coord::tile pos) {
	//TODO: translate pos somehow, so that buildings get placed centered to pos
	//TODO: set terrain to dirt

	if (this->placed) {
		throw Error("this object has already been placed.");
	}

	if (not this->fits(terrain, pos)) {
		return false;
	}

	this->terrain = terrain;
	this->set_position(pos);
	this->occupied_chunk_count = 0;

	bool chunk_known = false;


	//set pointers to this object on each terrain tile
	//where the building will stand and block the ground

	coord::tile temp_pos = this->start_pos;

	while (temp_pos.ne < this->end_pos.ne) {
		while (temp_pos.se < this->end_pos.se) {
			TerrainChunk *chunk = terrain->get_chunk(temp_pos);

			if (chunk == nullptr) {
				continue;
			}

			for (int c = 0; c < this->occupied_chunk_count; c++) {
				if (this->occupied_chunk[c] == chunk) {
					chunk_known = true;
				}
			}

			if (not chunk_known) {
				this->occupied_chunk[this->occupied_chunk_count] = chunk;
				this->occupied_chunk_count += 1;
			} else {
				chunk_known = false;
			}

			int tile_pos = chunk->tile_position(temp_pos);
			chunk->object[tile_pos] = this;

			//set the building underground to 10 = dirt-grass-whatever
			//TODO: in future, do this when buliding has fully been built, not earlier.
			chunk->set_tile(tile_pos, 10);

			temp_pos.se++;
		}
		temp_pos.se = this->start_pos.se;
		temp_pos.ne++;
	}

	//add object to the "base-chunk" for drawing
	occupied_chunk[0]->object_list.push_back(this);

	this->placed = true;
	return true;
}


bool TerrainObject::draw() {
	texture->draw(this->draw_pos.to_camgame(), false, 0, this->player);

	return true;
}

/**
* tests whether this terrain object will fit at the given position.
*
* @param terrain: the terrain where the check will be performed.
* @param pos: the base position.
* @returns true when the object fits, false otherwise.
*/
bool TerrainObject::fits(Terrain *terrain, coord::tile pos) {

	this->set_position(pos);

	coord::tile check_pos = this->start_pos;

	while (check_pos.ne < end_pos.ne) {
		while (check_pos.se < end_pos.se) {
			TerrainObject *test = terrain->get_object(check_pos);
			if (test != nullptr) {
				return false;
			}
			check_pos.se += 1;
		}
		check_pos.se = start_pos.se;
		check_pos.ne += 1;
	}
	return true;
}

/**
* set and calculate object start and end positions.
*
* @param pos: the center position of the building
*
* set the center position to "middle",
* start_pos is % and end_pos = &
*
* for a building, the # tile will be "middle":
*        @              @           @
*      @   @          @   @      %#   &
*    @   @   @      %   #   &       @
*  %   #   @   &      @   @
*    @   @   @          @
*      @   @
*        @
*/
void TerrainObject::set_position(coord::tile pos) {
	this->start_pos = pos;
	this->start_pos.ne -= (this->size.ne_length - 1) / 2;
	this->start_pos.se -= (this->size.se_length - 1) / 2;

	this->end_pos = this->start_pos;
	this->end_pos.ne += this->size.ne_length;
	this->end_pos.se += this->size.se_length;

	//TODO: fix this university-hardcoding..
	coord::tile drawpos_tile = this->start_pos;
	drawpos_tile.ne += 4;
	//log::dbg("drawpos: ne=%lu, se=%lu", drawpos_tile.ne, drawpos_tile.se);

	this->draw_pos = drawpos_tile.to_tile3().to_phys3();
}

} //namespace engine
