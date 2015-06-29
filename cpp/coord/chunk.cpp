// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#define GEN_IMPL_CHUNK_CPP
#include "chunk.h"

#include "tile.h"

namespace openage {
namespace coord {

tile chunk::to_tile(tile_delta pos_on_chunk) {
	tile result;
	result.ne = static_cast<tile_t>(ne) * chunk::tiles_per_chunk + pos_on_chunk.ne;
	result.se = static_cast<tile_t>(se) * chunk::tiles_per_chunk + pos_on_chunk.se;
	return result;
}

} // namespace coord
} // namespace openage
