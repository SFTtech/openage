#error "this file is a template for creating coordinate classes"

#include <stdint.h>

#define MEMBERS x, y
#define RELATIVE_TYPE my_system_delta
#define ABSOLUTE_TYPE my_system
#define SCALAR_TYPE int16_t

namespace openage {
namespace engine {
namespace coord {

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

} //namespace coord
} //namespace engine
} //namespace openage

#ifndef KEEP_DEFS
#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE
#endif //KEEP_DEFS
