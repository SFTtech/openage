#ifndef _ENGINE_COORD_CAMHUD_H_
#define _ENGINE_COORD_CAMHUD_H_

#include "decl.h"

#define MEMBERS x, y
#define SCALAR_TYPE pixel_t
#define ABSOLUTE_TYPE camhud
#define RELATIVE_TYPE camhud_delta

namespace engine {
namespace coord {

struct camhud {
	pixel_t x, y;

	#include "ops/abs.h"

	window to_window();
};

struct camhud_delta {
	pixel_t x, y;

	#include "ops/rel.h"

	window_delta to_window();
};

#include "ops/free.h"

#ifdef GEN_IMPL_CAMHUD_CPP
#include "ops/impl.h"
#endif //GEN_IMPL_CAMHUD_CPP

} //namespace coord
} //namespace engine

#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE

#endif //_ENGINE_COORD_CAMHUD_H_
