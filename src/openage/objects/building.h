#ifndef _OPENAGE_OBJECTS_BUILDING_H_
#define _OPENAGE_OBJECTS_BUILDING_H_

#include "../../engine/coord/tile.h"
#include "../../engine/texture.h"

namespace openage {

struct building {
	engine::coord::tile pos;
	unsigned player;
	engine::Texture *tex;
};

} //namespace openage

#endif //_OPENAGE_OBJECTS_BUILDING_H_
