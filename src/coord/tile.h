#ifndef _ENGINE_COORD_TILE_H_
#define _ENGINE_COORD_TILE_H_

#include "decl.h"

#include "phys2.h"

#define MEMBERS ne, se
#define SCALAR_TYPE tile_t
#define ABSOLUTE_TYPE tile
#define RELATIVE_TYPE tile_delta

namespace engine {
namespace coord {

struct tile {
	tile_t ne, se;

	#include "ops/abs.h"

	tile3 to_tile3(tile_t up = 0);
	phys2 to_phys2(phys2_delta frac = {phys_per_tile / 2, phys_per_tile / 2});
	chunk to_chunk();
	tile_delta get_pos_on_chunk();
};

struct tile_delta {
	tile_t ne, se;

	#include "ops/rel.h"

	tile3_delta to_tile3(tile_t up = 0);
	tile to_tile();
};

#include "ops/free.h"

#ifdef GEN_IMPL_TILE_CPP
#include "ops/impl.h"
#endif //GEN_IMPL_TILE_CPP

} //namespace coord
} //namespace engine

#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE

#endif //_ENGINE_COORD_TILE_H_
