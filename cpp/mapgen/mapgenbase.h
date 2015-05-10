// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_MAPGEN_MAPGENBASE_ENGINE_H_
#define OPENAGE_MAPGEN_MAPGENBASE_ENGINE_H_

#include <stddef.h>
#include <vector>

#include "../coord/tile.h"

namespace openage {
namespace mapgen {

// TODO this should be a parameter to allow the correct map sizes
// For example, tiny, small and medium maps are of size 120x120, 144x144 and 168x168
// There's no chunk size which divides all these sizes evenly

class MapGenBase {
public:

	MapGenBase(int chunks_per_size, int chunk_size, uint64_t seed);
	~MapGenBase() {};

	// this function is the only thing, which an engine needs to implement
	virtual int *get_map(int32_t x, int32_t y) = 0;
	coord::tile_delta get_size() const;

	std::vector<int> map;
	coord::tile_delta size;
protected:
	uint64_t seed;
};

} // namespace mapgen
} // namespace openage

#endif
