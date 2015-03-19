// Copyright 2013-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_VEC3_H_
#define OPENAGE_COORD_VEC3_H_

#include "decl.h"

#define MEMBERS x, y, z
#define SCALAR_TYPE int64_t
#define ABSOLUTE_TYPE vec3
#define RELATIVE_TYPE vec3_delta

namespace openage {
namespace coord {

struct vec3 {
	int64_t x, y, z;

	#include "ops/abs.h"
};

struct vec3_delta {
	int64_t x, y, z;

	#include "ops/rel.h"
};

#include "ops/free.h"

#ifdef GEN_IMPL_VEC3_CPP
#include "ops/impl.h"
#endif //GEN_IMPL_VEC3_CPP

} // namespace coord
} // namespace openage

#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE

#endif
