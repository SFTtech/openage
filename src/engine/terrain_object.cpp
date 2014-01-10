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
	this->size = { 4, 4 };
	this->player = player;

	this->occupied_chunk_count = 0;
}


TerrainObject::~TerrainObject() {}


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

	coord::tile temp_pos = this->start_pos;
	for (unsigned i = 0; i < this->size.ne_length ; i++) {
		for (unsigned j = 0; j < this->size.se_length; j++) {
			TerrainChunk *chunk = terrain->get_chunk(temp_pos);

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
			temp_pos.se += 1;
		}
		temp_pos.se = this->start_pos.se;
		temp_pos.ne += 1;
	}

	occupied_chunk[0]->object_list.push_back(this);

	this->placed = true;
	return true;
}


bool TerrainObject::draw() {
	texture->draw(this->start_pos.to_tile3().to_phys3().to_camgame(), false, 0, this->player);

	return true;
}

bool TerrainObject::fits(Terrain *terrain, coord::tile pos) {
	//tile is the base position.
	//look at this->size and the tile occumations
	//to determine whether the object fits on the ground.

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

void TerrainObject::set_position(coord::tile pos) {
	this->start_pos = pos;
	//this->start_pos.ne -= this->size.ne_length / 2;
	//this->start_pos.se -= this->size.se_length / 2;

	this->end_pos = this->start_pos;
	this->end_pos.ne += this->size.ne_length;
	this->end_pos.se += this->size.se_length;
}

} //namespace engine
