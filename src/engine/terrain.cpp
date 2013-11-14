#include "terrain.h"



#include "engine.h"
#include "texture.h"
#include "../log/log.h"
#include "../util/error.h"
#include "../util/misc.h"

namespace openage {
namespace engine {

coord::camera_delta tile_halfsize = {48.f, 24.f};

Terrain::Terrain(unsigned int size, size_t maxtextures, size_t maxblendmodes, int *priority_list) : texture_count(maxtextures), blendmode_count(maxblendmodes), terrain_id_priority_map(priority_list) {
	//calculate the number of tiles for the given (tile) height of the rhombus.

	/* height = 5:
	0          #
	1        #   #
	2      #   *   #
	3    #   #   #   #
	4  #   #   #   #   #
	5    #   #   #   *
	6      #   #   #
	7        #   #
	8          #
	count = 25

	count = n + 2*(n-1) + 2*(n-2) + ... + 2*1
	      = n + 2*(sum(i=1->n-1) {i})
	      = n + 2*(((n-1)^2 + (n-1))/2)
	      = n + ((n-1)^2 + (n-1))
	      = n + (n-1) + (n-1)^2
	      = 2*n + (n-1)^2 -1
	      = 2*n + (n^2 - 2n + 1) -1
	      = n^2
	      => lol. i could have seen that earlier..
	*/
	this->size       = size;
	this->num_rows   = 2*size - 1;
	this->tile_count = size * size;

	this->tiles = new int[this->tile_count];
	this->textures = new engine::Texture*[maxtextures];
	this->blendmasks = new engine::Texture*[maxblendmodes];

	this->blending_enabled = true;

	//set the terrain id to 0 by default.
	for (unsigned int i = 0; i < this->tile_count; i++) {
		this->tiles[i] = 0;
	}

	log::dbg("created terrain: %lu size, %lu rows, %lu tiles", this->size, this->num_rows, this->tile_count);
}

Terrain::~Terrain() {
	delete[] this->tiles;
	delete[] this->textures;
}

coord::tileno_delta const neigh_offsets[] = {
	{ 1, -1, 0},
	{ 1,  0, 0},
	{ 1,  1, 0},
	{ 0,  1, 0},
	{-1,  1, 0},
	{-1,  0, 0},
	{-1, -1, 0},
	{ 0, -1, 0}
};

void Terrain::draw() {
	const bool respect_diagonal_influence = true;
	const bool respect_adjacent_influence = true;

	coord::tileno tileno = {0, 0, 0};
	for (; tileno.ne < (int) this->size; tileno.ne++) {
		for (tileno.se = 0; tileno.se < (int) this->size; tileno.se++) {
			int terrain_id = this->get_tile(tileno);
			int base_priority = this->terrain_id_priority_map[terrain_id];

			auto texture = this->textures[terrain_id];
			int sub_id = get_subtexture_id(tileno, texture->atlas_dimensions);

			//draw the base texture
			texture->draw(coord::tileno_to_phys(tileno), false, sub_id);

			if (!this->blending_enabled) {
				continue;
			}

			//blendomatic!!111
			// see doc/media/blendomatic for the idea behind this.

			//storage for influences by neighbor tile
			struct influence_meta {
				int direction;
				int priority;
				int terrain_id;
			};

			int influence_tids[8];
			int num_influences = 0;

			auto influences = new struct influence_meta[this->texture_count];
			for (unsigned int k = 0; k < this->texture_count; k++) {
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
				coord::tileno neigh_pos = tileno + neigh_offsets[neigh_id];

				if (neigh_pos.ne < 0 || neigh_pos.ne >= (int)this->size || neigh_pos.se < 0 || neigh_pos.se >= (int)this->size) {
					//this neighbor is on the neighbor chunk or not existing, skip it for now.
					continue;
				}

				int neighbor_terrain_id = this->get_tile(neigh_pos);
				int neighbor_priority = this->terrain_id_priority_map[neighbor_terrain_id];

				//neighbor only interesting if it's a different terrain than @
				//comment by mci_e: this is also checked in the next if condition.
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
				delete[] influences;
				continue;
			}

			//influences to consider when drawing overlays
			struct influence_meta draw_influences[8];

			//remove the influences terrain_id grouping
			for (int k = 0; k < num_influences; k++) {
				draw_influences[k] = influences[ influence_tids[k] ];
			}
			delete[] influences;

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
					int anti_redundancy_offset = util::mod<coord::tileno_t>(tileno.ne + tileno.se, 4);
					adjacent_mask_id += anti_redundancy_offset;
				}

				//TODO:
				int blend_mode = 5;     //get_blending_mode(priority, base)

				bool adjacent_mask_existing = false;

				if (adjacent_mask_id < 0) {
					if (respect_adjacent_influence && !respect_diagonal_influence && binfdiagonal == 0) {
						throw util::Error("influence detected with unknown directions: %u = 0x%02X", binf, binf);
					}
				} else if (respect_adjacent_influence) {
					draw_masks[mask_count].mask_id    = adjacent_mask_id;
					draw_masks[mask_count].blend_mode = blend_mode;
					mask_count += 1;
					adjacent_mask_existing = true;
				}

				if (respect_diagonal_influence && !adjacent_mask_existing) {
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
							mask_count += 1;
						}
					}
				}
			}

			//log::dbg2("drawing %d masks for tile %d,%d", mask_count, i, j);
			for (int k = 0; k < mask_count; k++) {
				//mask, to be applied on neighbor_terrain_id tile
				this->blendmasks[draw_masks[k].blend_mode]->draw(coord::tileno_to_phys(tileno), false, draw_masks[k].mask_id);
				//this->textures[neighbor_terrain_id]->draw(x, y, false, sub_id);
			}
		}
	}
}


/**
returns the terrain subtexture id for a given position.

this function returns always the right value, so that neighbor tiles
of the same terrain (like grass-grass) are matching (without blendomatic).
*/
unsigned Terrain::get_subtexture_id(coord::tileno pos, unsigned atlas_size) {
	unsigned result = 0;

	result += util::mod<coord::tileno_t>(pos.se, atlas_size);
	result *= atlas_size;
	result += util::mod<coord::tileno_t>(pos.ne, atlas_size);

	return result;
}

void Terrain::set_tile(coord::tileno pos, int tile) {
	tiles[tile_position(pos)] = tile;
}

int Terrain::get_tile(coord::tileno pos) {
	return tiles[tile_position(pos)];
}

/**
calculates the memory position of a given tile location.

give this function isometric coordinates, it returns the tile index.

# is a single terrain tile:

         3
       2   #
     1   #   #
x= 0   #   *   #
     #   #   #   #
y= 0   #   #   #
     1   #   #
       2   #
         3

for example, * is at position (2, 1)
the returned index would be 6 (count for each x row, starting at y=0)
*/
size_t Terrain::tile_position(coord::tileno pos) {
	if (pos.ne >= (int) this->size || pos.ne < 0 || pos.se >= (int) this->size || pos.se < 0) {
		throw util::Error("requested tile (%ld, %ld) that's not on this terrain.", pos.ne, pos.se);
	}

	return pos.se * this->size + pos.ne;
}

/**
calculates the memory position of a given diagonal tile location.

this does not respect the isometric coordinates, it's for drawn rows.

@OBSOLETE FOR NOW
*/
size_t Terrain::tile_position_diag(unsigned int row, unsigned int offset) {
	int so_far; //number of tiles in memory before the row
	unsigned int in_row; //number of tiles in the destination row

	if (row > this->num_rows - 1) {
		throw util::Error("Requested row %u, but there are only %lu rows in terrain.", row, this->num_rows);
	}

	//calculation if selected tile is in the upper half
	if (row <= this->num_rows/2) {
		so_far = (row*row + row)/2;
		in_row = row + 1;
	}
	//else the selected tile is in the lower half
	else {
		int brow = (this->num_rows - 1) - row + 1;
		so_far = this->size*this->size - ((brow*brow + brow)/2);
		in_row = brow;
	}

	if (offset > in_row-1) {
		throw util::Error("Requested tile %d of row %d which has only %d tiles", offset, row, in_row);
	}

	size_t position = so_far + offset;

	return position;
}

size_t Terrain::get_tile_count() {
	return this->tile_count;
}

void Terrain::set_texture(size_t index, engine::Texture *t) {
	this->textures[index] = t;
}

engine::Texture *Terrain::get_texture(size_t index) {
	return this->textures[index];
}

size_t Terrain::tiles_in_row(unsigned int row) {
	unsigned int in_row; //number of tiles in the destination row

	if (row > this->num_rows - 1) {
		throw util::Error("Requested row %u, but there are only %lu rows in terrain.", row, this->num_rows);
	}

	if (row <= this->num_rows/2) {
		in_row = row + 1;
	}
	else {
		in_row = (this->num_rows - 1) - row + 1;
	}
	return in_row;
}

size_t Terrain::get_size() {
	return this->size;
}

void Terrain::set_mask(unsigned int modeid, engine::Texture *m) {
	this->blendmasks[modeid] = m;
}

} //namespace engine
} //namespace openage
