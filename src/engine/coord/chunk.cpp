#define GEN_IMPL_CHUNK_CPP
#include "chunk.h"

#include "tile.h"

namespace openage {
namespace engine {
namespace coord {

tile chunk::to_tile(tile_delta pos_on_chunk) {
	tile result;
	result.ne = (((tile_t) ne) << tiles_per_chunk_bits) + pos_on_chunk.ne;
	result.se = (((tile_t) se) << tiles_per_chunk_bits) + pos_on_chunk.se;
	return result;
}

} //namespace coord
} //namespace engine
} //namespace openage
