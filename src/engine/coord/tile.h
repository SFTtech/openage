#ifndef _ENGINE_COORD_TILE_H_
#define _ENGINE_COORD_TILE_H_

#include "decl.h"

#define MEMBERS ne, se
#define SCALAR_TYPE tile_t
#define ABSOLUTE_TYPE tile
#define RELATIVE_TYPE tile_delta

namespace openage {
namespace engine {
namespace coord {

struct tile {
	tile_t ne, se;

	#include "ops/abs.h"

	tile3 to_tile3(tile_t up);
	phys2 to_phys2(phys2_delta frac);
	chunk to_chunk();
	tile_delta get_pos_on_chunk();
};

struct tile_delta {
	tile_t ne, se;

	#include "ops/rel.h"

	tile3_delta to_tile3(tile_t up);
};

#include "ops/free.h"

#ifdef GEN_IMPL
#undef GEN_IMPL
#include "ops/impl.h"
#endif //GEN_IMPL

} //namespace coord
} //namespace engine
} //namespace openage

#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE

#endif //_ENGINE_COORD_TILE_H_
