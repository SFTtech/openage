#ifndef _ENGINE_COORD_PHYS3_H_
#define _ENGINE_COORD_PHYS3_H_

#include "decl.h"

#define MEMBERS ne, se, up
#define SCALAR_TYPE phys_t
#define ABSOLUTE_TYPE phys3
#define RELATIVE_TYPE phys3_delta

namespace openage {
namespace engine {
namespace coord {

struct phys3 {
	phys_t ne, se, up;

	#include "ops/abs.h"

	phys2 to_phys2();
	camgame to_camgame();
	tile3 to_tile3();
	phys3_delta get_fraction();
};

struct phys3_delta {
	phys_t ne, se, up;

	#include "ops/rel.h"

	phys2_delta to_phys2();
	camgame_delta to_camgame();
};

#include "ops/free.h"

#ifdef GEN_IMPL_PHYS3_CPP
#include "ops/impl.h"
#endif //GEN_IMPL_PHYS3_CPP

} //namespace coord
} //namespace engine
} //namespace openage

#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE

#endif //_ENGINE_COORD_PHYS3_H_
