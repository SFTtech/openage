#include "gamestate.h"

namespace openage {

using namespace engine;

std::vector<building> buildings;

terrain_t editor_current_terrain = 0;

Terrain *terrain;
TerrainChunk **terrain_chunks;

Texture *gaben, *university;

} //namespace openage
