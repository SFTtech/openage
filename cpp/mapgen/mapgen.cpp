// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "mapgen.h"

namespace openage {
namespace mapgen {

MapGen::MapGen(int chunks_per_side)
	:
	map((chunks_per_side * chunk_size) * (chunks_per_side * chunk_size), 0),
	size{chunks_per_side * chunk_size, chunks_per_side * chunk_size} {}

int *MapGen::generate() {
	return this->map.data();
}

coord::tile_delta MapGen::get_size() const {
	return this->size;
}

} // namespace mapgen
} // namespace openage
