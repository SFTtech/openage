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
	this->chunk = std::map<coord::chunk, TerrainChunk *>{};
}

Terrain::~Terrain() {}

} //namespace engine
