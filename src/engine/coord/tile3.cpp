#define GEN_IMPL_TILE3_CPP
#include "tile3.h"

#include "tile.h"
#include "phys3.h"

namespace engine {
namespace coord {

tile tile3::to_tile() {
	return tile {ne, se};
}

tile_delta tile3_delta::to_tile() {
	return tile_delta {ne, se};
}

phys3 tile3::to_phys3(phys3_delta frac) {
	phys3 result;
	result.ne = (((phys_t) ne) << phys_t_radix_pos) + frac.ne;
	result.se = (((phys_t) se) << phys_t_radix_pos) + frac.se;
	result.up = (((phys_t) up) << phys_t_radix_pos) + frac.up;
	return result;
}

} //namespace coord
} //namespace engine
