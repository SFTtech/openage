#ifndef _ENGINE_COORD_PHYS2_H_
#define _ENGINE_COORD_PHYS2_H_

#include "decl.h"

#define MEMBERS ne, se
#define SCALAR_TYPE phys_t
#define ABSOLUTE_TYPE phys2
#define RELATIVE_TYPE phys2_delta

namespace engine {
namespace coord {

struct phys2 {
	phys_t ne, se;

	#include "ops/abs.h"

	phys3 to_phys3(phys_t up = 0);
	tile to_tile();
	phys2_delta get_fraction();
};

struct phys2_delta {
	phys_t ne, se;

	#include "ops/rel.h"

	phys3_delta to_phys3(phys_t up = 0);

	//allowing conversions of phys deltas to tile deltas isn't such a great
	//idea. you never know whether a phys change of 0.9 will _actually_
	//change the tile.
};

#include "ops/free.h"

#ifdef GEN_IMPL_PHYS2_CPP
#include "ops/impl.h"
#endif //GEN_IMPL_PHYS2_CPP

} //namespace coord
} //namespace engine

#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE

#endif //_ENGINE_COORD_PHYS2_H_
