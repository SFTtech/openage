// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <vector>

#include "mapgenbase.h"

namespace openage {
namespace mapgen {


MapGenBase::MapGenBase(int chunk_size, uint64_t seed)
	:
	map(chunk_size * chunk_size, 5),
	chunk_size(chunk_size),
	size{chunk_size,chunk_size} {}

coord::tile_delta MapGenBase::get_size() const {
	return this->size;
}
} // namespace mapgen
} // namespace openage
