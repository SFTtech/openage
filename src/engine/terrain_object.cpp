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
	if (this->occupied_chunk_count == 0 || not this->placed) {
		return;
	}

	coord::tile temp_pos = this->start_pos;

	while (temp_pos.ne < this->end_pos.ne) {
		while (temp_pos.se < this->end_pos.se) {
			TerrainChunk *chunk = terrain->get_chunk(temp_pos);

			if (chunk == nullptr) {
				continue;
			}

			int tile_pos = chunk->tile_position_neigh(temp_pos);
			chunk->get_data(tile_pos)->obj = nullptr;

			temp_pos.se++;
		}
		temp_pos.se = this->start_pos.se;
		temp_pos.ne++;
	}

	this->occupied_chunk_count = 0;
	this->placed = false;
}


/**
* binds the TerrainObject to a certain TerrainChunk.
*
* @param terrain: the terrain where the object will be placed onto.
* @param pos: (tile) position of the (nw,sw) corner
* @returns true when the object was placed, false when it did not fit at pos.
*/
bool TerrainObject::place(Terrain *terrain, coord::tile pos) {
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

			int tile_pos = chunk->tile_position_neigh(temp_pos);
			chunk->get_data(tile_pos)->obj = this;

			temp_pos.se++;
		}
		temp_pos.se = this->start_pos.se;
		temp_pos.ne++;
	}

	this->placed = true;
	return true;
}

/**
* sets all the ground below the object to a terrain id.
*
* @param id: the terrain id to which the ground is set
* @param additional: amount of additional space arround the building
*
*/
void TerrainObject::set_ground(int id, int additional) {

	if (not this->placed) {
		throw Error("setting ground for object that is not placed yet.");
	}

	coord::tile temp_pos = this->start_pos;
	temp_pos.ne -= additional;
	temp_pos.se -= additional;

	while (temp_pos.ne < this->end_pos.ne + additional) {
		while (temp_pos.se < this->end_pos.se + additional) {
			TerrainChunk *chunk = terrain->get_chunk(temp_pos);

			if (chunk == nullptr) {
				continue;
			}

			size_t tile_pos = chunk->tile_position_neigh(temp_pos);
			chunk->get_data(tile_pos)->terrain_id = id;
			temp_pos.se++;
		}
		temp_pos.se = this->start_pos.se - additional;
		temp_pos.ne++;
	}
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
	//TODO: add underground checking (water needed? solid ground needed?)

	this->set_position(pos);

	coord::tile check_pos = this->start_pos;

	while (check_pos.ne < end_pos.ne) {
		while (check_pos.se < end_pos.se) {
			TerrainChunk *chunk = terrain->get_chunk(check_pos);
			if (chunk == nullptr) {
				return false;
			} else {
				TerrainObject *test = chunk->get_data(check_pos)->obj;
				if (test != nullptr) {
					return false;
				}
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

	this->draw_pos = drawpos_tile.to_phys2().to_phys3();
}


bool TerrainObject::operator <(const TerrainObject *other) {
	return this->start_pos.ne < other->start_pos.ne
		|| (this->start_pos.ne == other->start_pos.ne
		    && this->start_pos.se < other->start_pos.se);
}

} //namespace engine
