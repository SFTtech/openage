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
	
	Static(int chunks_per_size, int chunk_size);
	~Static();

	int *generate();

};

} // namespace mapgen
} // namespace openage

#endif
