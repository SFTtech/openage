#ifndef _ENGINE_COORD_TILE3_H_
#define _ENGINE_COORD_TILE3_H_

#include "decl.h"

#define MEMBERS ne, se, up
#define SCALAR_TYPE tile_t
#define ABSOLUTE_TYPE tile3
#define RELATIVE_TYPE tile3_delta

namespace openage {
namespace engine {
namespace coord {

struct tile3 {
	tile_t ne, se, up;

	#include "ops/abs.h"

	tile to_tile();
	phys3 to_phys3(phys3_delta frac);
};

struct tile3_delta {
	tile_t ne, se, up;

	#include "ops/rel.h"

	tile_delta to_tile();
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

#endif //_ENGINE_COORD_TILE3_H_
