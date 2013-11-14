#include "terrain.h"



#include "engine.h"
#include "texture.h"
#include "../log/log.h"
#include "../util/error.h"
#include "../util/misc.h"

namespace openage {
namespace engine {

coord::camera_delta tile_halfsize = {48.f, 24.f};

Terrain::Terrain(unsigned int size, unsigned int maxtextures) {

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

	//set the tile index to 0 by default.
	for (unsigned int i = 0; i < this->tile_count; i++) {
		this->tiles[i] = 0;
	}

	log::dbg("created terrain: %lu size, %lu rows, %lu tiles", this->size, this->num_rows, this->tile_count);
}

Terrain::~Terrain() {
	delete[] this->tiles;
	delete[] this->textures;
}

void Terrain::draw() {
	coord::tileno tileno = {0, 0, 0};
	for (; tileno.ne < (int) this->size; tileno.ne++) {
		for (tileno.se = 0; tileno.se < (int) this->size; tileno.se++) {
			int terrain_id = this->get_tile(tileno);
			auto texture = this->textures[terrain_id];
			//TODO determine atlas size from texture
			//for now, asume a 10x10 atlas.
			int sub_id = this->get_subtexture_id(tileno, 10);

			texture->draw(coord::tileno_to_phys(tileno), false, sub_id);
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


} //namespace engine
} //namespace openage
