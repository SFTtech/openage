// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_MAPGEN_MAPGEN_H_
#define OPENAGE_MAPGEN_MAPGEN_H_

#include <stddef.h>
#include <vector>

#include "../coord/tile.h"

namespace openage {
namespace mapgen {

// TODO this should be a parameter to allow the correct map sizes
// For example, tiny, small and medium maps are of size 120x120, 144x144 and 168x168
// There's no chunk size which divides all these sizes evenly
constexpr int chunk_size = 16;

class MapGen {
public:
	MapGen(int chunks_per_size);

	int *generate();
	coord::tile_delta get_size() const;

private:
	std::vector<int> map;
	coord::tile_delta size;
};

} // namespace mapgen
} // namespace openage

#endif
