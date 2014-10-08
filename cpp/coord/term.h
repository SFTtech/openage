#ifndef _ENGINE_COORD_TERM_H_
#define _ENGINE_COORD_TERM_H_

#include "decl.h"

#define MEMBERS x, y
#define SCALAR_TYPE term_t
#define ABSOLUTE_TYPE term
#define RELATIVE_TYPE term_delta

namespace openage {
namespace console {
class Console;
} //namespace console

namespace coord {

struct term {
	term_t x, y;

	#include "ops/abs.h"

	camhud to_camhud(console::Console *e);
};

struct term_delta {
	term_t x, y;

	#include "ops/rel.h"
};

#include "ops/free.h"

#ifdef GEN_IMPL_TERM_CPP
#include "ops/impl.h"
#endif //GEN_IMPL_TERM_CPP

} //namespace coord
} //namespace openage

#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE

#endif //_ENGINE_COORD_TERM_H_
