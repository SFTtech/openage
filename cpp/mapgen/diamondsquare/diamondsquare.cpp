// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <vector>

#include "diamondsquare.h"
#include "heightmap.h"
#include "../../log/log.h"
#include "../mapgen.h"
#include "../mapgenbase.h"

namespace openage {
namespace mapgen {


Diamondsquare::Diamondsquare(int chunks_per_side, int chunk_size)
	:
	MapGenBase(chunks_per_side,chunk_size) {}
	  
int *Diamondsquare::generate() {
	Heightmap heightmap(this->size);
	heightmap.generate();

	for (int y = 0; y < this->size.se; y++) {
		for (int x = 0; x < this->size.ne; x++) {
			float height = heightmap.tile(x, y);
			int index = this->size.ne * y + x;
			if (height > 0) {
				this->map[index] = MapGen::Terrain::Dirt_1;
			} else if (height == 0) {
				this->map[index] = MapGen::Terrain::Water_Dark;
			}
		}
	}

	return this->map.data();
}

} // namespace mapgen
} // namespace openage
