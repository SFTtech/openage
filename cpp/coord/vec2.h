#ifndef OPENAGE_ENGINE_COORD_VEC2_H_5394AF3242684DFD9B97FCC5B33A599D
#define OPENAGE_ENGINE_COORD_VEC2_H_5394AF3242684DFD9B97FCC5B33A599D

#include "decl.h"

#define MEMBERS x, y
#define SCALAR_TYPE int64_t
#define ABSOLUTE_TYPE vec2
#define RELATIVE_TYPE vec2_delta

namespace openage {
namespace coord {

struct vec2 {
	int64_t x, y;

	#include "ops/abs.h"
};

struct vec2_delta {
	int64_t x, y;

	#include "ops/rel.h"
};

#include "ops/free.h"

#ifdef GEN_IMPL_VEC2_CPP
#include "ops/impl.h"
#endif //GEN_IMPL_VEC2_CPP

} //namespace coord
} //namespace openage

#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE

#endif //_ENGINE_COORD_VEC2_H_
