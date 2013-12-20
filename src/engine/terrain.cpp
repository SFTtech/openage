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
	this->chunks = std::map<coord::chunk, TerrainChunk *>{};
}

Terrain::~Terrain() {}

void Terrain::attach_chunk(TerrainChunk *new_chunk, coord::chunk position) {
	new_chunk->set_terrain(this);
	this->chunks[position] = new_chunk;
}

void Terrain::draw() {
	for (auto &chunk : this->chunks) {
		//TODO: respect chunk position
		chunk.second->draw();
	}
}


} //namespace engine
