#ifndef _OPENAGE_GAMESTATE_H_
#define _OPENAGE_GAMESTATE_H_

#include <unordered_set>

#include "../engine/terrain.h"
#include "../engine/audio/sound.h"

namespace openage {

/**
 * the buildings that were placed on the terrain.
 */
extern std::unordered_set<engine::TerrainObject *> buildings;

/**
 * the selected id for drawing terrain.
 */
extern engine::terrain_t editor_current_terrain;

/**
 * the game's terrain
 */
extern engine::Terrain *terrain;

//TODO this is here temporarily (obviously...)
extern engine::Texture *gaben, *university;

extern engine::audio::Sound *build_uni_sound, *destroy_uni_sound0, *destroy_uni_sound1;

} //namespace openage

#endif //_OPENAGE_GAMESTATE_H_
