#ifndef _OPENAGE_GAMESTATE_H_
#define _OPENAGE_GAMESTATE_H_

#include <unordered_set>

#include "../engine/terrain.h"

namespace openage {

extern std::unordered_set<engine::TerrainObject *> buildings;

extern engine::terrain_t editor_current_terrain;

extern engine::Terrain *terrain;

//TODO this is here temporarily (obviously...)
extern engine::Texture *gaben, *university;

} //namespace openage

#endif //_OPENAGE_GAMESTATE_H_
