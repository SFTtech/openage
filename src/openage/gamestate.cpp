#include "gamestate.h"

namespace openage {

using namespace engine;

std::unordered_set<TerrainObject *> buildings;

terrain_t editor_current_terrain = 0;

Terrain *terrain;

Texture *gaben, *university;

audio::Sound *build_uni, *destroy_uni0, *destroy_uni1;

} //namespace openage
