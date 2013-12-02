#define GEN_IMPL
#include "tile.h"

#include "tile3.h"
#include "chunk.h"
#include "phys2.h"

namespace openage {
namespace engine {
namespace coord {

tile3 tile::to_tile3(tile_t up) {
	return tile3 {ne, se, up};
}

tile3_delta tile_delta::to_tile3(tile_t up) {
	return tile3_delta {ne, se, up};
}

phys2 tile::to_phys2(phys2_delta frac) {
	phys2 result;
	result.ne = (((phys_t) ne) << phys_t_radix_pos) + frac.ne;
	result.se = (((phys_t) se) << phys_t_radix_pos) + frac.se;
	return result;
}

chunk tile::to_chunk() {
	chunk result;
	result.ne = (ne >> tiles_per_chunk_bits);
	result.se = (se >> tiles_per_chunk_bits);
	return result;
}

tile_delta tile::get_pos_on_chunk() {
	tile_delta result;
	result.ne = ne - ((ne >> tiles_per_chunk_bits) << tiles_per_chunk_bits);
	result.se = se - ((se >> tiles_per_chunk_bits) << tiles_per_chunk_bits);
	return result;
}

} //namespace coord
} //namespace engine
} //namespace openage
