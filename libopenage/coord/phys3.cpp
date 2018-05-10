// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#include <cmath>

#define GEN_IMPL_PHYS3_CPP
#include "phys3.h"

#include "phys2.h"
#include "vec2.h"
#include "tile3.h"
#include "../terrain/terrain.h"
#include "../engine.h"

namespace openage {
namespace coord {

phys2 phys3::to_phys2() const {
	return phys2 {ne, se};
}

phys2_delta phys3_delta::to_phys2() const {
	return phys2_delta {ne, se};
}

camgame phys3::to_camgame() const {

	//determine the phys3 position relative to the camera position
	phys3_delta relative_phys = *this - Engine::get_coord_data()->camgame_phys;

	return relative_phys.to_camgame().as_absolute();
}

camgame_delta phys3_delta::to_camgame() const {
	coord_data* engine_coord_data{ Engine::get_coord_data() };

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
	result.x = (pixel_t) util::div(scaled.x * engine_coord_data->tile_halfsize.x, settings::phys_per_tile);
	result.y = (pixel_t) util::div(scaled.y * engine_coord_data->tile_halfsize.y, settings::phys_per_tile);

	return result;
}

tile3 phys3::to_tile3() const {
	tile3 result;
	result.ne = (ne >> settings::phys_t_radix_pos);
	result.se = (se >> settings::phys_t_radix_pos);
	result.up = (up >> settings::phys_t_radix_pos);
	return result;
}

phys3_delta phys3::get_fraction() {
	phys3_delta result;

	// define a bitmask that keeps the last n bits
	decltype(result.ne) bitmask = ((1 << settings::phys_t_radix_pos) - 1);

	result.ne = (ne & bitmask);
	result.se = (se & bitmask);
	result.up = (up & bitmask);
	return result;
}

phys_t distance(const phys3 &a, const phys3 &b) {
	phys_t dx = a.ne - b.ne;
	phys_t dy = a.se - b.se;
	return std::sqrt(dx * dx + dy * dy);
}

phys3_delta normalize(const phys3_delta &a, const phys_t &length) {
	phys3_delta result = (a * length) / std::sqrt(a.ne * a.ne + a.se * a.se);
	return result;
}

} // namespace coord
} // namespace openage
