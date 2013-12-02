#error "this file is a template for creating coordinate classes"

#ifndef _ENGINE_COORD_[NAME]_H_
#define _ENGINE_COORD_[NAME]_H_

#include "decl.h"

#define MEMBERS [memberlist]
#define SCALAR_TYPE [scalartype]
#define ABSOLUTE_TYPE [absname]
#define RELATIVE_TYPE [relname]

namespace openage {
namespace engine {
namespace coord {

struct [absname] {
	[scalartype] [memberlist];

	#include "ops/abs.h"
};

struct [relname] {
	[scalartype] [memberlist];

	#include "ops/rel.h"
};

#include "ops/free.h"

#ifdef GEN_IMPL_[NAME]_CPP
#include "ops/impl.h"
#endif //GEN_IMPL_[NAME]_CPP

} //namespace coord
} //namespace engine
} //namespace openage

#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE

#endif //_ENGINE_COORD_[NAME]_H_
