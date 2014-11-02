#ifndef OPENAGE_COORD_TILE3_H_
#define OPENAGE_COORD_TILE3_H_

#include "decl.h"
#include "phys3.h"

#define MEMBERS ne, se, up
#define SCALAR_TYPE tile_t
#define ABSOLUTE_TYPE tile3
#define RELATIVE_TYPE tile3_delta

namespace openage {
namespace coord {

struct tile3 {
	tile_t ne, se, up;

	#include "ops/abs.h"

	tile to_tile() const;
	phys3 to_phys3(phys3_delta frac = {settings::phys_per_tile / 2, settings::phys_per_tile / 2, 0});
};

struct tile3_delta {
	tile_t ne, se, up;

	#include "ops/rel.h"

	tile_delta to_tile() const;
};

#include "ops/free.h"

#ifdef GEN_IMPL_TILE3_CPP
#include "ops/impl.h"
#endif //GEN_IMPL_TILE3_CPP

} //namespace coord
} //namespace openage

#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE

#endif
