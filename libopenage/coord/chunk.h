// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_CHUNK_H_
#define OPENAGE_COORD_CHUNK_H_

#include "decl.h"
#include "ne_se_up_coord.h"

namespace openage {
namespace coord {

struct chunk : public absolute_ne_se_up_coord<chunk, chunk_delta, chunk_t, 2> {
	chunk() = default;
	chunk(chunk_t ne, chunk_t se);

	tile to_tile(tile_delta pos_on_chunk);
};

struct chunk_delta : public relative_ne_se_up_coord<chunk, chunk_delta, chunk_t, 2> {
	chunk_delta() = default;
	chunk_delta(chunk_t ne, chunk_t se);
};

} // namespace coord
} // namespace openage

#endif
