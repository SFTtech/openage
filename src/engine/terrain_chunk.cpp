#include "terrain_chunk.h"

#include "engine.h"
#include "texture.h"
#include "log.h"
#include "util/error.h"
#include "util/misc.h"
#include "coord/tile.h"
#include "coord/tile3.h"
#include "coord/phys3.h"
#include "coord/camgame.h"

namespace engine {


TerrainChunk::TerrainChunk() {
	this->size       = chunk_size;
	this->tile_count = size * size;
	this->manually_created = true;

	//the ids of terraintype pieces on the terrain
	//each element describes what terrain is at this position.
	this->tiles = new int[this->tile_count];

	//set all tiles on the terrain id to 0 by default.
	for (unsigned int i = 0; i < this->tile_count; i++) {
		this->tiles[i] = 0;
	}

	//initialize all neighbors as nonexistant
	for (int i = 0; i < 8; i++) {
		this->neighbors.neighbor[i] = nullptr;
	}

	log::dbg("created terrain chunk: %lu size, %lu tiles",
	         this->size,
	         this->tile_count);
}

TerrainChunk::~TerrainChunk() {
	delete[] this->tiles;
}

void TerrainChunk::draw(coord::chunk chunk_pos) {
	const bool respect_diagonal_influence = true;
	const bool respect_adjacent_influence = true;

	//storage for influences by neighbor tile
	struct influence_meta {
		int direction;
		int priority;
		int terrain_id;
	};
	auto influences = new struct influence_meta[this->terrain->terrain_type_count];

	coord::tile tilepos = {0, 0};
	for (; tilepos.ne < (ssize_t) this->size; tilepos.ne++) {
		for (tilepos.se = 0; tilepos.se < (ssize_t) this->size; tilepos.se++) {
			int terrain_id = this->get_tile(tilepos);

			if (terrain_id >= (ssize_t)this->terrain->terrain_type_count) {
				throw Error("unknown terrain id=%d requested for drawing", terrain_id);
			} else if (terrain_id < 0) {
				continue;
			}

			int base_priority = this->terrain->priority(terrain_id);
			auto base_texture = this->terrain->texture(terrain_id);
			int sub_id = this->terrain->get_subtexture_id(tilepos, base_texture->atlas_dimensions, chunk_pos);

			//position, where the tile is drawn
			coord::tile tile_chunk_pos = chunk_pos.to_tile(tilepos.get_pos_on_chunk());
			coord::camgame draw_pos = tile_chunk_pos.to_tile3().to_phys3().to_camgame();
			//draw the base texture
			base_texture->draw(draw_pos, false, sub_id);

			if (!this->terrain->blending_enabled) {
				continue;
			}

			//blendomatic!!111
			// see doc/media/blendomatic for the idea behind this.

			int influence_tids[8];
			int num_influences = 0;

			for (unsigned int k = 0; k < this->terrain->terrain_type_count; k++) {
				influences[k].direction = 0;
				influences[k].priority = -1;
			}

			/* neighbor ids:
			      0
			    7   1
			  6   @   2
			    5   3
			      4
			*/


			//first step: gather information about possible influences
			//look at every neighbor tile for that
			for (int neigh_id = 0; neigh_id < 8; neigh_id++) {
				if (!respect_diagonal_influence && (neigh_id % 2) == 0) {
					continue;
				}

				if (!respect_adjacent_influence && (neigh_id % 2) == 1) {
					continue;
				}

				//calculate the pos of the neighbor tile
				coord::tile neigh_pos = tilepos + neigh_offsets[neigh_id];
				int neighbor_terrain_id = this->get_tile_neigh(neigh_pos);
				if (neighbor_terrain_id < 0) {
					continue;
				}

				//get the neighbor's blending priority
				int neighbor_priority = this->terrain->priority(neighbor_terrain_id);

				//neighbor only interesting if it's a different terrain than @
				//if it is the same priority, the neigh priority is not higher.
				if (neighbor_terrain_id != terrain_id) {

					//neighbor draws over the base if it's priority is greater
					if (neighbor_priority > base_priority) {
						auto influence = &influences[neighbor_terrain_id];

						//this terrain id has no influence yet:
						if (influence->priority == -1) {
							influence_tids[num_influences] = neighbor_terrain_id;
							num_influences += 1;
						}

						//as tile i has influence for this priority
						// => bit i is set to 1 by 2^i
						influence->direction |= 1 << neigh_id;
						influence->priority = neighbor_priority;
						influence->terrain_id = neighbor_terrain_id;
					}
				}
			}

			//no blending necessary for this tile, it has no external influences.
			if (num_influences <= 0) {
				continue;
			}

			//influences to consider when drawing overlays
			struct influence_meta draw_influences[8];

			//remove the influences terrain_id grouping
			for (int k = 0; k < num_influences; k++) {
				draw_influences[k] = influences[ influence_tids[k] ];
			}

			//order the influences by their priority
			for (int k = 0; k < num_influences; k++) {
				struct influence_meta tmp_influence = draw_influences[k];

				int l = k - 1;
				while (l >= 0 && draw_influences[l].priority > tmp_influence.priority) {
					draw_influences[l + 1] = draw_influences[l];
					l -= 1;
				}

				draw_influences[l + 1] = tmp_influence;
			}

			struct draw_mask {
				int mask_id;
				int blend_mode;
				int terrain_id;
			};

			int mask_count = 0;
			struct draw_mask draw_masks[4+4]; //maximum 4 diagonal and 4 adjacent masks

			//for each possible influence (max 8 as we have 8 neighbors)
			for (int k = 0; k < num_influences; k++) {
				unsigned int binf = draw_influences[k].direction & 0xFF; //0b11111111
				if (binf == 0) {
					continue;
				}

				int neighbor_terrain_id = draw_influences[k].terrain_id;
				int adjacent_mask_id = -1;

				//log::dbg2("priority %d => mask %d, terrain %d", k, binf, neighbor_terrain_id);

				/* neighbor ids:
				      0
				    7   1      => 8 neighbors that can have influence on
				  6   @   2         the mask id selection.
				    5   3
				      4
				*/

				//ignore diagonal influences for adjacent influences
				int binfadjacent = binf & 0xAA; //0b10101010
				int binfdiagonal = binf & 0x55; //0b01010101

				//comment by mic_e TODO replace this by a lookup table
				if (respect_adjacent_influence) {
					switch (binfadjacent) {
					case 0x08:  //0b00001000
						adjacent_mask_id = 0;  //0..3
						break;
					case 0x02:  //0b00000010
						adjacent_mask_id = 4;  //4..7
						break;
					case 0x20:  //0b00100000
						adjacent_mask_id = 8;  //8..11
						break;
					case 0x80:  //0b10000000
						adjacent_mask_id = 12; //12..15
						break;
					case 0x22:  //0b00100010
						adjacent_mask_id = 20;
						break;
					case 0x88:  //0b10001000
						adjacent_mask_id = 21;
						break;
					case 0xA0:  //0b10100000
						adjacent_mask_id = 22;
						break;
					case 0x82:  //0b10000010
						adjacent_mask_id = 23;
						break;
					case 0x28:  //0b00101000
						adjacent_mask_id = 24;
						break;
					case 0x0A:  //0b00001010
						adjacent_mask_id = 25;
						break;
					case 0x2A:  //0b00101010
						adjacent_mask_id = 26;
						break;
					case 0xA8:  //0b10101000
						adjacent_mask_id = 27;
						break;
					case 0xA2:  //0b10100010
						adjacent_mask_id = 28;
						break;
					case 0x8A:  //0b10001010
						adjacent_mask_id = 29;
						break;
					case 0xAA:  //0b10101010
						adjacent_mask_id = 30;
						break;
					}
				}

				//if it's the plain adjacent mask, use all of the 4 possible masks.
				if (adjacent_mask_id <= 12 && adjacent_mask_id % 4 == 0) {
					int anti_redundancy_offset = util::mod<coord::tile_t>(tilepos.ne + tilepos.se, 4);
					adjacent_mask_id += anti_redundancy_offset;
				}

				//get the blending mode (the mask selection) for this transition
				//the mode is dependent on the two meeting terrain types
				int blend_mode = this->terrain->get_blending_mode(terrain_id, neighbor_terrain_id);

				if (adjacent_mask_id < 0) {
					if (respect_adjacent_influence && !respect_diagonal_influence && binfdiagonal == 0) {
						throw Error("influence detected with unknown directions: %u = 0x%02X", binf, binf);
					}
				} else if (respect_adjacent_influence) {
					draw_masks[mask_count].mask_id    = adjacent_mask_id;
					draw_masks[mask_count].blend_mode = blend_mode;
					draw_masks[mask_count].terrain_id = neighbor_terrain_id;
					mask_count += 1;
				}

				if (respect_diagonal_influence) {
					for (int l = 0; l < 4; l++) {
						//generate the neighbor id bit.
						int bdiaginf = 1 << (l*2);
						const int diag_mask_id_map[4] = {18, 16, 17, 19};

						// l == 0: pos = 0b000000001, mask = 18
						// l == 1: pos = 0b000000100, mask = 16
						// l == 2: pos = 0b000010000, mask = 17
						// l == 3: pos = 0b001000000, mask = 19
						if (binf & bdiaginf) {
							draw_masks[mask_count].mask_id    = diag_mask_id_map[l];
							draw_masks[mask_count].blend_mode = blend_mode;
							draw_masks[mask_count].terrain_id = neighbor_terrain_id;
							mask_count += 1;
						}
					}
				}
			}

			for (int k = 0; k < mask_count; k++) {
				//mask, to be applied on neighbor_terrain_id tile
				auto draw_mask = &draw_masks[k];
				auto overlay_texture = this->terrain->texture(draw_mask->terrain_id);

				//TODO: get the neighbor's correct subtexture id on other chunk
				int neighbor_sub_id = this->terrain->get_subtexture_id(tilepos, overlay_texture->atlas_dimensions, chunk_pos);

				//the texture used for masking
				auto mask_tex = this->terrain->blending_mask(draw_mask->blend_mode);
				overlay_texture->activate_alphamask(mask_tex, draw_mask->mask_id);
				overlay_texture->draw(draw_pos, false, neighbor_sub_id);
				overlay_texture->disable_alphamask();
			}
		}
	}
	delete[] influences;
}

/**
set the terrain id of a given tile on this chunk.
*/
void TerrainChunk::set_tile(coord::tile pos, int tile) {
	this->tiles[this->tile_position(pos)] = tile;
}

/**
get the terrain id of a given tile position on this chunk.

segfaults if pos is not on this chunk, beware.
use get_tile_neigh instead if query is not chunksafe.
@see get_tile_neigh()
*/
int TerrainChunk::get_tile(coord::tile pos) {
	return this->tiles[this->tile_position(pos)];
}

/**
get the terrain id of a given tile position relative to this chunk.

also queries neighbors if the position is not on this chunk.
*/
int TerrainChunk::get_tile_neigh(coord::tile pos) {
	//determine the neighbor id by the given position
	int neighbor_id = this->neighbor_id_by_pos(pos);

	//if the location is not on the current chunk, the neighbor id is != -1
	if (neighbor_id != -1) {

		//get the chunk where the requested neighbor tile lies on.
		TerrainChunk *neigh_chunk = this->neighbors.neighbor[neighbor_id];

		//this neighbor does not exist, so the tile does not exist.
		if (neigh_chunk == nullptr) {
			//log::dbg("neighbor chunk not found");
			return -1;
		}

		//get position of tile on neighbor
		coord::tile pos_on_neighbor;
		pos_on_neighbor.ne = util::mod<coord::tile_t>(pos.ne, this->size);
		pos_on_neighbor.se = util::mod<coord::tile_t>(pos.se, this->size);

		return neigh_chunk->get_tile(pos_on_neighbor);
	}
	//the position lies on the current chunk.
	else {
		return this->get_tile(pos);
	}
}

/**
get the chunk neighbor id by a given position not lying on this chunk.

neighbor ids:

   ne
--
/|
      0
    7   1
  6   @   2
    5   3
      4
\|
--
  se
*/
int TerrainChunk::neighbor_id_by_pos(coord::tile pos) {
	int neigh_id = -1;

	if (pos.ne < 0) {
		if (pos.se < 0) {
			neigh_id = 6;
		}
		else if (pos.se >= (ssize_t)this->size) {
			neigh_id = 4;
		}
		else {
			neigh_id = 5;
		}
	}
	else if (pos.ne >= (ssize_t)this->size) {
		if (pos.se < 0) {
			neigh_id = 0;
		}
		else if (pos.se >= (ssize_t)this->size) {
			neigh_id = 2;
		}
		else {
			neigh_id = 1;
		}
	}
	else {
		if (pos.se < 0) {
			neigh_id = 7;
		}
		else if (pos.se >= (ssize_t)this->size) {
			neigh_id = 3;
		}
		else {
			neigh_id = -1;
		}
	}
	return neigh_id;
}

/**
calculates the memory position of a given tile location.

give this function isometric coordinates, it returns the tile index.

# is a single terrain tile:

          3
        2   #
      1   #   #
ne= 0   #   *   #
      #   #   #   #
se= 0   #   #   #
      1   #   #
        2   #
          3

for example, * is at position (2, 1)
the returned index would be 6 (count for each ne row, starting at se=0)
*/
size_t TerrainChunk::tile_position(coord::tile pos) {
	if (this->neighbor_id_by_pos(pos) != -1) {
		throw Error("requested tile (%ld, %ld) that's not on this terrain.", pos.ne, pos.se);
	}

	return pos.se * this->size + pos.ne;
}

size_t TerrainChunk::get_tile_count() {
	return this->tile_count;
}

size_t TerrainChunk::get_size() {
	return this->size;
}

void TerrainChunk::set_terrain(Terrain *parent) {
	this->terrain = parent;
}

} //namespace engine
