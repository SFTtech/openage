#define GEN_IMPL_PHYS2_CPP
#include "phys2.h"

#include "phys3.h"
#include "tile.h"

namespace engine {
namespace coord {

phys3 phys2::to_phys3(phys_t up) {
	return phys3 {ne, se, up};
}

phys3_delta phys2_delta::to_phys3(phys_t up) {
	return phys3_delta {ne, se, up};
}

tile phys2::to_tile() {
	tile result;
	result.ne = (ne >> phys_t_radix_pos);
	result.se = (se >> phys_t_radix_pos);
	return result;
}

phys2_delta phys2::get_fraction() {
	phys2_delta result;
	result.ne = ne - ((ne >> phys_t_radix_pos) << phys_t_radix_pos);
	result.se = se - ((se >> phys_t_radix_pos) << phys_t_radix_pos);
	return result;
}

} //namespace coord
} //namespace engine
