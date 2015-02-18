// Copyright 2013-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_TILE_H_
#define OPENAGE_COORD_TILE_H_
#include <functional>

#include "../util/misc.h"
#include "decl.h"
#include "phys2.h"

#define MEMBERS ne, se
#define SCALAR_TYPE tile_t
#define ABSOLUTE_TYPE tile
#define RELATIVE_TYPE tile_delta

namespace openage {
namespace coord {

struct tile {
	tile_t ne, se;

	#include "ops/abs.h"

	tile3 to_tile3(tile_t up = 0) const;
	phys2 to_phys2(phys2_delta frac = {settings::phys_per_tile / 2, settings::phys_per_tile / 2}) const;
	chunk to_chunk() const;
	tile_delta get_pos_on_chunk() const;
};

struct tile_delta {
	tile_t ne, se;

	#include "ops/rel.h"

	tile3_delta to_tile3(tile_t up = 0) const;
	tile to_tile();
};

#include "ops/free.h"

#ifdef GEN_IMPL_TILE_CPP
#include "ops/impl.h"
#endif //GEN_IMPL_TILE_CPP

} //namespace coord
} //namespace openage

#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE
namespace std{
template<>
struct hash<openage::coord::tile>{
    size_t operator ()(const openage::coord::tile& pos) const{
	size_t nehash = hash<openage::coord::tile_t>{}(pos.ne);
	size_t sehash = hash<openage::coord::tile_t>{}(pos.se);
	return openage::util::rol<size_t, 1>(nehash) ^ sehash;	
    }
};
} //namespace std
#endif
