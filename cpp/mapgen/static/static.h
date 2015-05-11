// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_MAPGEN_STATIC_H_
#define OPENAGE_MAPGEN_STATIC_H_

#include <stddef.h>
#include <vector>

#include "../../coord/tile.h"
#include "../mapgenbase.h"

namespace openage {
namespace mapgen {

/* generates a map full of Dirt */

class Static : public MapGenBase {
public:

	Static(int chunk_size,uint64_t seed);
	~Static();

	int *get_map(int32_t x, int32_t y);

};

} // namespace mapgen
} // namespace openage

#endif
