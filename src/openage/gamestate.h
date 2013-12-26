#ifndef _OPENAGE_GAMESTATE_H_
#define _OPENAGE_GAMESTATE_H_

#include <vector>

#include "objects/building.h"
#include "objects/terrain.h"
#include "../engine/terrain.h"

namespace openage {

extern std::vector<building> buildings;

extern terrain_t editor_current_terrain;

extern engine::Terrain *terrain;

extern engine::TerrainChunk *first_chunk;


//TODO this is here temporarily (obviously...)
extern engine::Texture *gaben, *university;

} //namespace openage

#endif //_OPENAGE_GAMESTATE_H_
