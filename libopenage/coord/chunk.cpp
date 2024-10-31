// Copyright 2016-2024 the openage authors. See copying.md for legal info.

#include "chunk.h"

#include "coord/tile.h"


namespace openage {
namespace coord {

tile_delta chunk_delta::to_tile(tile_t tiles_per_chunk) const {
	return tile_delta{this->ne * tiles_per_chunk, this->se * tiles_per_chunk};
}

tile chunk::to_tile(tile_t tiles_per_chunk) const {
	return tile{this->ne * tiles_per_chunk, this->se * tiles_per_chunk};
}

} // namespace coord
} // namespace openage
