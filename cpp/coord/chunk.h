// Copyright 2013-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_CHUNK_H_
#define OPENAGE_COORD_CHUNK_H_

#include "decl.h"

#define MEMBERS ne, se
#define SCALAR_TYPE chunk_t
#define ABSOLUTE_TYPE chunk
#define RELATIVE_TYPE chunk_delta

namespace openage {
namespace coord {

struct chunk {
	chunk_t ne, se;

	#include "ops/abs.h"

	tile to_tile(tile_delta pos_on_chunk);
};

struct chunk_delta {
	chunk_t ne, se;

	#include "ops/rel.h"
};

#include "ops/free.h"

#ifdef GEN_IMPL_CHUNK_CPP
#include "ops/impl.h"
#endif //GEN_IMPL_CHUNK_CPP

} //namespace coord
} //namespace openage

#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE

#endif
