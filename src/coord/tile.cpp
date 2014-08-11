#define GEN_IMPL_TILE_CPP
#include "tile.h"

#include "tile3.h"
#include "chunk.h"
#include "phys2.h"

namespace openage {
namespace coord {

tile3 tile::to_tile3(tile_t up) {
	return tile3 {ne, se, up};
}

tile3_delta tile_delta::to_tile3(tile_t up) {
	return tile3_delta {ne, se, up};
}

phys2 tile::to_phys2(phys2_delta frac) {
	phys2 result;
	result.ne = (((phys_t) ne) << settings::phys_t_radix_pos) + frac.ne;
	result.se = (((phys_t) se) << settings::phys_t_radix_pos) + frac.se;
	return result;
}

chunk tile::to_chunk() {
	chunk result;
	result.ne = (ne >> settings::tiles_per_chunk_bits);
	result.se = (se >> settings::tiles_per_chunk_bits);
	return result;
}

tile tile_delta::to_tile() {
	tile result;
	result.ne = this->ne;
	result.se = this->se;
	return result;
}

tile_delta tile::get_pos_on_chunk() {
	tile_delta result;

	// define a bitmask that keeps the last n bits
	decltype(result.ne) bitmask = ((1 << settings::phys_t_radix_pos) - 1);

	result.ne = (ne & bitmask);
	result.se = (se & bitmask);
	return result;
}

} //namespace coord
} //namespace openage
