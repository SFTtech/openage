#ifndef OPENAGE_COORD_WINDOW_H_
#define OPENAGE_COORD_WINDOW_H_

#include "decl.h"

#define MEMBERS x, y
#define SCALAR_TYPE pixel_t
#define ABSOLUTE_TYPE window
#define RELATIVE_TYPE window_delta

namespace openage {
namespace coord {

struct window {
	pixel_t x, y;

	#include "ops/abs.h"

	camgame to_camgame();
	camhud to_camhud();
};

struct window_delta {
	pixel_t x, y;

	#include "ops/rel.h"

	camgame_delta to_camgame();
	camhud_delta to_camhud();
};

#include "ops/free.h"

#ifdef GEN_IMPL_WINDOW_CPP
#include "ops/impl.h"
#endif //GEN_IMPL_WINDOW_CPP

} //namespace coord
} //namespace openage

#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE

#endif
