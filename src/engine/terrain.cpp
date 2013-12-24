#include "terrain.h"

#include <map>

#include "terrain_chunk.h"
#include "engine.h"
#include "texture.h"
#include "log.h"
#include "util/error.h"
#include "coord/camgame.h"
#include "coord/chunk.h"

namespace engine {

//TODO: get that from the convert script!
coord::camgame_delta tile_halfsize = {48, 24};


Terrain::Terrain() {
	this->chunks = std::map<coord::chunk, TerrainChunk *, coord_chunk_compare>{};
	this->blending_enabled = true;
}

Terrain::~Terrain() {}

void Terrain::attach_chunk(TerrainChunk *new_chunk, coord::chunk position) {
	new_chunk->set_terrain(this);
	this->chunks[position] = new_chunk;

	struct chunk_neighbors neigh = this->get_chunk_neighbors(position);
	for (int i = 0; i < 8; i++) {
		TerrainChunk *neighbor = neigh.neighbor[i];
		if (neighbor != nullptr) {
			neighbor->neighbors.neighbor[(i+4) % 8] = new_chunk;
		}
	}
}

TerrainChunk *Terrain::get_chunk(coord::chunk position) {
	//is this chunk stored?
	if (this->chunks.find(position) == this->chunks.end()) {
		return nullptr;
	}
	else {
		return this->chunks[position];
	}
}

void Terrain::draw() {
	for (auto &chunk : this->chunks) {
		//TODO: respect chunk position
		//coord::chunk pos = chunk.first;
		chunk.second->draw();
	}
}

/**
get the adjacent chunk neighbors.

chunk neighbor ids:
      0   / <- ne
    7   1
  6   @   2
    5   3
      4   \ <- se

   ne se
0:  1 -1
1:  1  0
2:  1  1
3:  0  1
4: -1  1
5: -1  0
6: -1 -1
7:  0 -1

*/
struct chunk_neighbors Terrain::get_chunk_neighbors(coord::chunk position) {
	struct chunk_neighbors ret;
	coord::chunk tmp_pos;

	constexpr int neighbor_pos_delta[8][2] = {
		{  1, -1},
		{  1,  0},
		{  1,  1},
		{  0,  1},
		{ -1,  1},
		{ -1,  0},
		{ -1, -1},
		{  0, -1},
	};

	for (int i = 0; i < 8; i++) {
		tmp_pos = position;
		//TODO: use the overloaded operators..
		tmp_pos.ne += neighbor_pos_delta[i][0];
		tmp_pos.se += neighbor_pos_delta[i][1];
		ret.neighbor[i] = this->get_chunk(tmp_pos);
	}

	return ret;
}


} //namespace engine
