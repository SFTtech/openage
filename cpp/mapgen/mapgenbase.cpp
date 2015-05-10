// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <vector>

#include "mapgenbase.h"

namespace openage {
namespace mapgen {


MapGenBase::MapGenBase(int chunks_per_side, int chunk_size, uint64_t seed)
	:
	map((chunks_per_side * chunk_size) * (chunks_per_side * chunk_size), 5),
	size{chunks_per_side * chunk_size, chunks_per_side * chunk_size} {}

coord::tile_delta MapGenBase::get_size() const {
	return this->size;
}
} // namespace mapgen
} // namespace openage
