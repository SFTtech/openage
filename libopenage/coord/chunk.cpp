// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#define GEN_IMPL_CHUNK_CPP
#include "chunk.h"

#include "tile.h"

namespace openage {
namespace coord {

chunk::chunk(chunk_t ne, chunk_t se)
{
	this->ne = ne;
	this->se = se;
}

chunk_delta::chunk_delta(chunk_t ne, chunk_t se)
{
	this->ne = ne;
	this->se = se;
}

tile chunk::to_tile(tile_delta pos_on_chunk) {
	tile result;
	result.ne = (((tile_t) ne) << settings::tiles_per_chunk_bits)
		+ pos_on_chunk.ne;
	result.se = (((tile_t) se) << settings::tiles_per_chunk_bits)
		+ pos_on_chunk.se;
	return result;
}

} // namespace coord
} // namespace openage
