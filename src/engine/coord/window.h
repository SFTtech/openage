#ifndef _ENGINE_COORD_WINDOW_H_
#define _ENGINE_COORD_WINDOW_H_

#include <stdint.h>

#define MEMBERS x, y
#define RELATIVE_TYPE window_delta
#define ABSOLUTE_TYPE window
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

#endif //_ENGINE_COORD_WINDOW_H_
