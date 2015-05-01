// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <vector>

#include "mapgen.h"
#include "heightmap.h"
#include "../log/log.h"

namespace openage {
namespace mapgen {


MapGen::MapGen(int chunks_per_side)
	:
	map((chunks_per_side * chunk_size) * (chunks_per_side * chunk_size), 5),
	size{chunks_per_side * chunk_size, chunks_per_side * chunk_size} {}

int *MapGen::generate() {
	Heightmap heightmap(this->size);
	heightmap.generate();

	for (int y = 0; y < this->size.se; y++) {
		for (int x = 0; x < this->size.ne; x++) {
			float height = heightmap.tile(x, y);
			int index = this->size.ne * y + x;
			if (height > 0) {
				this->map[index] = 0;
			} else if (height == 0) {
				this->map[index] = 16;
			}
		}
	}

	return this->map.data();
}

coord::tile_delta MapGen::get_size() const {
	return this->size;
}

} // namespace mapgen
} // namespace openage
