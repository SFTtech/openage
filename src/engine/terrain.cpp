#include "terrain.h"



#include "engine.h"
#include "texture.h"
#include "../log/log.h"
#include "../util/error.h"

namespace openage {
namespace engine {

Terrain::Terrain(unsigned int size, size_t maxtextures, size_t maxblendmodes) : texture_count(maxtextures), blendmode_count(maxblendmodes) {

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

void Terrain::render() {
	for (unsigned int i = 0; i < this->size; i++) {
		for (unsigned int j = 0; j < this->size; j++) {
			int terrain_id = this->tile_at(i, j);
			int sub_id = this->get_subtexture_id(i, j);
			int base_priority = 2;

			auto texture = this->textures[terrain_id];

			int x, y;
			int tw, th;
			texture->get_subtexture_size(sub_id, &tw, &th);

			x = i * (tw/2) + j * (tw/2);
			y = i * (th/2) - j * (th/2);

			//draw the base texture
			texture->draw(x, y, false, sub_id);


			if (!this->blending_enabled) {
				continue;
			}
			//blendomatic!!111
			// see doc/media/blendomatic for the idea behind this.

			//storage for influences by neighbor tile
			struct influence_meta {
				int direction;
				int terrain_id;
			};

			auto influences = new struct influence_meta[this->texture_count];
			for (int k = 0; k < this->texture_count; k++) {
				influences[k] = {0, 0};
			}

			//first step: gather information about possible influences
			//look at every neighbor tile for that
			for (int neigh_id = 0; neigh_id < 8; neigh_id++) {
				int neigh_x, neigh_y;
				int neighbor_priority = 8;

				//get the tile coordinates of the current neighbor
				switch (neigh_id) {
				case 0:
					neigh_x = i + 1;
					neigh_y = j - 1;
					break;
				case 1:
					neigh_x = i + 1;
					neigh_y = j;
					break;
				case 2:
					neigh_x = i + 1;
					neigh_y = j + 1;
					break;
				case 3:
					neigh_x = i;
					neigh_y = j + 1;
					break;
				case 4:
					neigh_x = i - 1;
					neigh_y = j + 1;
					break;
				case 5:
					neigh_x = i - 1;
					neigh_y = j;
					break;
				case 6:
					neigh_x = i - 1;
					neigh_y = j - 1;
					break;
				case 7:
					neigh_x = i;
					neigh_y = j - 1;
					break;
				default:
					throw util::Error("unknown neighbor requested!");
				}

				//log::dbg2("testing neighbor %d at %d %d", neigh_id, neigh_x, neigh_y);

				//neighbor only interesting if it's a different terrain than @
				if (neigh_x < 0 || neigh_x >= this->size || neigh_y < 0 || neigh_y >= this->size) {
					//this neighbor is on the neighbor chunk, skip it for now.
					continue;
				}

				int neighbor_terrain_id = this->tile_at(neigh_x, neigh_y);
				if (neighbor_terrain_id != terrain_id) {

					//neighbor draws over the base if it's priority is greater
					if (neighbor_priority > base_priority) {
						//as tile i has influence for this priority
						// => bit i is set to 1 by 2^i
						influences[neighbor_priority].direction |= 1 << neigh_id;
						influences[neighbor_priority].terrain_id = neighbor_terrain_id;
					}
				}
			}

			//for each possible priority
			for (int k = 0; k < this->texture_count; k++) {
				unsigned int binf = influences[k].direction & 0xFF; //0b11111111
				if (binf == 0) {
					continue;
				}

				int neighbor_terrain_id = influences[k].terrain_id;
				int adjacent_mask_id = -1;

				//log::dbg2("priority %d => mask %d, terrain %d", k, binf, neighbor_terrain_id);

				/*    0
				    7   1      => 8 neighbours that can have influence on
				  6   @   2         the mask id selection.
				    5   3
				      4
				*/

				//ignore diagonal influences for adjacent influences
				int binfadjacent = binf & 0xAA; //0b10101010
				int binfdiagonal = binf & 0x55; //0b01010101

				switch (binfadjacent) {
				case 0x08:  //0b00001000
					adjacent_mask_id = 0; //random 0..3
					break;
				case 0x02:  //0b00000010
					adjacent_mask_id = 4; //random 0..7
					break;
				case 0x20:  //0b00100000
					adjacent_mask_id = 8; //random 8..11
					break;
				case 0x80:  //0b10000000
					adjacent_mask_id = 12; //random 12..15
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

				int blendmode = 5;     //get_blending_mode(priority, base)

				if (adjacent_mask_id < 0) {
					if (binfdiagonal == 0) {
						throw util::Error("influence detected with unknown directions: %u = 0x%02X", binf, binf);
					} else {
						//ignore diagonal masks for now
						continue;
					}
				}

				//TODO: diagonal influences
				/*else {
				if (binf & 0b00000100) {
				}
				if (binf & 0b00010000) {
				}
				if (binf & 0b00000001) {
				}
				if (binf & 0b00100000) {
				}
				}
				*/

				//mask, to be applied on neighbor_terrain_id tile
				this->blendmasks[blendmode]->draw(x, y, false, adjacent_mask_id);
				//this->textures[neighbor_terrain_id]->draw(x, y, false, sub_id);
			}
			delete[] influences;
		}
	}
}


/**
returns the terrain subtexture id for a given position.

this function returns always the right value, so that neighbor tiles
of the same terrain (like grass-grass) are matching (without blendomatic).
*/
int Terrain::get_subtexture_id(unsigned int x, unsigned int y) {
	return ((x % 10) + (10 * (y % 10)));
}

void Terrain::set_tile_at(int index, int x, int y) {
	size_t pos = this->tile_position(x, y);
	this->tiles[pos] = index;
}

void Terrain::set_tile_at_row(int index, int row, int offset) {
	size_t pos = this->tile_position_diag(row, offset);
	this->tiles[pos] = index;
}

void Terrain::set_tile_at(int index, int position) {
	this->tiles[position] = index;
}

int Terrain::tile_at_row(int row, int offset) {
	size_t pos = tile_position_diag(row, offset);
	return this->tiles[pos];
}

int Terrain::tile_at(int x, int y) {
	size_t pos = tile_position(x, y);
	return this->tiles[pos];
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
size_t Terrain::tile_position(unsigned int x, unsigned int y) {
	if (x >= this->size || y >= this->size) {
		throw util::Error("requested tile (%d, %d) that's not on this terrain.", x, y);
	}

	return y * this->size + x;
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

void Terrain::set_texture(unsigned int index, engine::Texture *t) {
	this->textures[index] = t;
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
