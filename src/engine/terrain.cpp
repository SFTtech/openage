#include "terrain.h"



#include "engine.h"
#include "texture.h"
#include "../log/log.h"
#include "../util/error.h"

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

void Terrain::render() {
	for (unsigned int i = 0; i < this->size; i++) {
		for (unsigned int j = 0; j < this->size; j++) {
			int terrain_id = this->tile_at(i, j);
			int sub_id = this->get_subtexture_id(i, j);

			auto texture = this->textures[terrain_id];

			int x, y;
			int tw, th;
			texture->get_subtexture_size(sub_id, &tw, &th);

			x = i * (tw/2) + j * (tw/2);
			y = i * (th/2) - j * (th/2);

			texture->draw(x, y, false, sub_id);
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


} //namespace engine
} //namespace openage
