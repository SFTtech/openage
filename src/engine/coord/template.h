#error "this file is a template for creating coordinate classes"

#ifndef _ENGINE_COORD_[NAME]_H_
#define _ENGINE_COORD_[NAME]_H_

#include <stdint.h>

#define MEMBERS [memberlist]
#define SCALAR_TYPE [scalaralias]
#define RELATIVE_TYPE [name]_delta
#define ABSOLUTE_TYPE [name]

namespace openage {
namespace engine {
namespace coord {

using SCALAR_TYPE = [scalartype];

struct ABSOLUTE_TYPE;
struct RELATIVE_TYPE;

struct ABSOLUTE_TYPE {
	SCALAR_TYPE MEMBERS;

	#include "ops/abs.h"
};

struct RELATIVE_TYPE {
	SCALAR_TYPE MEMBERS;

	#include "ops/rel.h"
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

#endif //_ENGINE_COORD_[NAME]_H_
