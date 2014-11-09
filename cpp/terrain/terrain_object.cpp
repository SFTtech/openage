// Copyright 2013-2014 the openage authors. See copying.md for legal info.

#include "terrain_object.h"

#include "terrain.h"
#include "terrain_chunk.h"
#include "../engine.h"
#include "../log.h"
#include "../texture.h"
#include "../coord/tile.h"
#include "../coord/tile3.h"
#include "../coord/phys3.h"
#include "../coord/camgame.h"
#include "../util/error.h"

namespace openage {

TerrainObject::TerrainObject(Texture *tex,
                             coord::tile_delta foundation_size,
                             unsigned player, int sound_id_destruction)
	:
	sound_id_destruction{sound_id_destruction},
	placed{false},
	texture{tex},
	player{player},
	occupied_chunk_count{0} {
	this->size = foundation_size;
}


TerrainObject::~TerrainObject() {}


void TerrainObject::remove() {
	if (this->occupied_chunk_count == 0 or not this->placed) {
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


bool TerrainObject::place(Terrain *terrain, coord::tile pos) {
	if (this->placed) {
		throw util::Error("this object has already been placed.");
	}

	// storing the position happens in here:
	if (not this->fits(terrain, pos)) {
		return false;
	}

	this->terrain = terrain;
	this->occupied_chunk_count = 0;

	bool chunk_known = false;


	// set pointers to this object on each terrain tile
	// where the building will stand and block the ground

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


void TerrainObject::set_ground(int id, int additional) {

	if (not this->placed) {
		throw util::Error("setting ground for object that is not placed yet.");
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
	texture->draw(this->draw_pos.to_camgame(), PLAYERCOLORED, false, 0, this->player);

	return true;
}


bool TerrainObject::fits(Terrain *terrain, coord::tile pos) {
	this->set_position(pos);

	coord::tile check_pos = this->start_pos;

	while (check_pos.ne < end_pos.ne) {
		while (check_pos.se < end_pos.se) {
			TerrainChunk *chunk = terrain->get_chunk(check_pos);
			if (chunk == nullptr) {
				return false;
			} else {
				// get tile data
				auto tile = chunk->get_data(check_pos);

				// is the terrain underground suitable?
				// TODO: water needed? solid ground needed?
				if (tile->terrain_id < 0) {
					return false;
				}

				// is another object blocking that tile?
				if (tile->obj != nullptr) {
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


void TerrainObject::set_position(coord::tile pos) {

	this->start_pos = pos;
	this->end_pos   = this->start_pos + this->size;
	this->draw_pos  = (this->start_pos.to_phys2() + this->size.to_phys2() / 2).to_phys3();

}


bool TerrainObject::operator <(const TerrainObject &other) {

	if (this == &other) {
		return false;
	}

	auto this_ne    = this->start_pos.ne;
	auto this_se    = this->start_pos.se;
	auto other_ne   = other.start_pos.ne;
	auto other_se   = other.start_pos.se;

	auto this_ypos  = this_ne  - this_se;
	auto other_ypos = other_ne - other_se;

	if (this_ypos < other_ypos) {
		return false;
	}
	else if (this_ypos == other_ypos) {
		if (this_ne > other_ne) {
			return false;
		}
		else if (this_ne == other_ne) {
			return this_se > other_se;
		}
	}

	return true;
}

} //namespace openage
