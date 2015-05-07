// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <vector>

#include "mapgenbase.h"

namespace openage {
namespace mapgen {


MapGenBase::MapGenBase(int chunks_per_side, int chunk_size)
	:
	map((chunks_per_side * chunk_size) * (chunks_per_side * chunk_size), 5),
	size{chunks_per_side * chunk_size, chunks_per_side * chunk_size} {}

std::vector<int> MapGenBase::get_map() {
	return this->map;
}

coord::tile_delta MapGenBase::get_size() const {
	return this->size;
}
} // namespace mapgen
} // namespace openage
