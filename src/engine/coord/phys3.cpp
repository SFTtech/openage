#define GEN_IMPL_PHYS3_CPP
#include "phys3.h"

#include "phys2.h"
#include "vec2.h"
#include "tile3.h"
#include "../terrain.h"
#include "../engine.h"

namespace engine {
namespace coord {

phys2 phys3::to_phys2() {
	return phys2 {ne, se};
}

phys2_delta phys3_delta::to_phys2() {
	return phys2_delta {ne, se};
}

camgame phys3::to_camgame() {
	//determine the phys3 position relative to the camera position
	phys3_delta relative_phys = *this - camgame_phys;

	return relative_phys.to_camgame().as_absolute();
}

camgame_delta phys3_delta::to_camgame() {
	//apply transformation matrix to relative_phys, to get 'scaled':
	//                  (ne)
	// (x) = (+1 +1 +0) (se)
	// (y) = (+1 -1 +1) (up)
	vec2 scaled;
	scaled.x = +1 * this->ne +1 * this->se +0 * this->up;
	scaled.y = +1 * this->ne -1 * this->se +1 * this->up;

	//remove scaling factor from scaled, to get result
	//scaling factor: w/2 for x, h/2 for y
	//and the (1 << 16) fixed-point scaling factor for both.
	camgame_delta result;
	result.x = (pixel_t) (scaled.x * tile_halfsize.x / phys_per_tile);
	result.y = (pixel_t) (scaled.y * tile_halfsize.y / phys_per_tile);

	return result;
}

tile3 phys3::to_tile3() {
	tile3 result;
	result.ne = (ne >> phys_t_radix_pos);
	result.se = (se >> phys_t_radix_pos);
	result.up = (up >> phys_t_radix_pos);
	return result;
}

phys3_delta phys3::get_fraction() {
	phys3_delta result;
	result.ne = ne - ((ne >> phys_t_radix_pos) << phys_t_radix_pos);
	result.se = se - ((se >> phys_t_radix_pos) << phys_t_radix_pos);
	result.up = up - ((up >> phys_t_radix_pos) << phys_t_radix_pos);
	return result;
}

} //namespace coord
} //namespace engine
