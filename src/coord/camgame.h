#ifndef _ENGINE_COORD_CAMGAME_H_
#define _ENGINE_COORD_CAMGAME_H_

#include "decl.h"

#define MEMBERS x, y
#define SCALAR_TYPE pixel_t
#define ABSOLUTE_TYPE camgame
#define RELATIVE_TYPE camgame_delta

namespace openage {
namespace coord {

struct camgame {
	pixel_t x, y;

	#include "ops/abs.h"

	phys3 to_phys3(phys_t up = 0);
	window to_window();
};

struct camgame_delta {
	pixel_t x, y;

	#include "ops/rel.h"

	phys3_delta to_phys3(phys_t up = 0);
	window_delta to_window();
};

#include "ops/free.h"

#ifdef GEN_IMPL_CAMGAME_CPP
#include "ops/impl.h"
#endif //GEN_IMPL_CAMGAME_CPP

} //namespace coord
} //namespace openage

#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE

#endif //_ENGINE_COORD_CAMGAME_H_
