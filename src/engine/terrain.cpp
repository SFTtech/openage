#include "terrain.h"



#include "engine.h"
#include "texture.h"
#include "../log/log.h"
#include "../util/error.h"

namespace openage {
namespace engine {

Terrain::Terrain(unsigned int size) {

	//calculate the number of tiles for the given (tile) height of the rhombus.

	/* height = 5:
	0      #
	1     # #
	2    # * #
	3   # # # #
	4  # # # # #
	5   # # # *
	6    # # #
	7     # #
	8      #
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

	//set the tile index to 0 by default.
	for (unsigned int i = 0; i < this->tile_count; i++) {
		this->tiles[i] = 0;
	}

	log::dbg("created terrain: %lu size, %lu rows, %lu tiles", this->size, this->num_rows, this->tile_count);
}

Terrain::~Terrain() {
	delete[] this->tiles;
}

void Terrain::render() {
	for (unsigned int i = 0; i < this->num_rows; i++) {
		for (unsigned int j = 0; j < this->tiles_in_row(i); j++) {
			int terrain_id = this->tile_at(i, j);

			int x, y, space;
			int tw, th;
			this->texture->get_subtexture_size(terrain_id, &tw, &th);

			space = (this->size - this->tiles_in_row(i)) * (tw/2);
			x = space + j * tw;
			y = i * (th/2);

			this->texture->draw(x, y, false, terrain_id);
		}
	}
}

void Terrain::set_tile_at(int index, int row, int offset) {
	size_t pos = this->tile_position(row, offset);
	this->tiles[pos] = index;
}

void Terrain::set_tile_at(int index, int position) {
	this->tiles[position] = index;
}

int Terrain::tile_at(int row, int offset) {
	size_t pos = tile_position(row, offset);
	return this->tiles[pos];
}

/**
calculates the memory position of a given tile location.
*/
size_t Terrain::tile_position(unsigned int row, unsigned int offset) {
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

void Terrain::set_texture(engine::Texture *t) {
	this->texture = t;
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


} //namespace engine
} //namespace openage
