// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <vector>

#include "static.h"
#include "../../log/log.h"
#include "../mapgen.h"
#include "../mapgenbase.h"

namespace openage {
namespace mapgen {


Static::Static(int chunks_per_side, int chunk_size)
	:
	MapGenBase(chunks_per_side,chunk_size) {}
	  
int *Static::generate() {
  
	for (int y = 0; y < this->size.se; y++) {
		for (int x = 0; x < this->size.ne; x++) {
			int index = this->size.ne * y + x;
			this->map[index] = MapGen::Terrain::Dirt_1;
		}
	}

	return this->map.data();
}

} // namespace mapgen
} // namespace openage
