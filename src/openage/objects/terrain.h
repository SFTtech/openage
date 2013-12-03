#ifndef _OPENAGE_OBJECTS_TERRAIN_H_
#define _OPENAGE_OBJECTS_TERRAIN_H_

#include <stdint.h>
#include <stdlib.h>

namespace openage {

using terrain_t = size_t;

//a list of all terrain slp ids. we'll get them from the .dat files later.
extern int terrain_ids[]; 
extern size_t terrain_texture_count;

} //namespace openage

#endif //_OPENAGE_OBJECTS_TERRAIN_H_

