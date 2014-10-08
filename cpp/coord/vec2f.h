#ifndef _ENGINE_COORD_VEC2F_H_
#define _ENGINE_COORD_VEC2F_H_

#include "decl.h"

#define MEMBERS x, y
#define SCALAR_TYPE float
#define ABSOLUTE_TYPE vec2f
#define RELATIVE_TYPE vec2f_delta

namespace openage {
namespace coord {

struct vec2f {
	float x, y;

	#include "ops/abs.h"
};

struct vec2f_delta {
	float x, y;

	#include "ops/rel.h"
};

#include "ops/free.h"

#ifdef GEN_IMPL_VEC2F_CPP
#include "ops/impl.h"
#endif //GEN_IMPL_VEC2F_CPP

} //namespace coord
} //namespace openage

#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE

#endif //_ENGINE_COORD_VEC2F_H_
