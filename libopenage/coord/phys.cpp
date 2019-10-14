// Copyright 2016-2019 the openage authors. See copying.md for legal info.

#include "phys.h"

#include "coordmanager.h"
#include "tile.h"
#include "pixel.h"
#include "../terrain/terrain.h"
#include "../util/math.h"


namespace openage::coord {


tile3 phys3::to_tile3() const {
	return tile3{this->ne.to_int(), this->se.to_int(), this->up.to_int()};
}


tile phys2::to_tile() const {
	return tile{this->ne.to_int(), this->se.to_int()};
}


phys2 phys3::to_phys2() const {
	return phys2{this->ne, this->se};
}


double phys2_delta::length() const {
	return std::hypot(this->ne.to_double(), this->se.to_double());
}


phys2_delta phys2_delta::normalize(double length) const {
	return *this * (length / this->length());
}


phys3_delta phys2_delta::to_phys3() const {
	return phys3_delta{this->ne, this->se, 0};
}


double phys2::distance(phys2 other) const {
	return (*this - other).length();
}


phys3 phys2::to_phys3(const Terrain &/* terrain */, phys_t altitude) const {
	// TODO: once terrain elevations have been implemented,
	//       query the terrain elevation at {ne, se}.
	phys_t elevation = 0;

	return phys3{this->ne, this->se, elevation + altitude};
}


camgame_delta phys3_delta::to_camgame(const CoordManager &mgr) const {
	// apply transformation matrix to phys3_delta, to get 'scaled':
	//                   (ne)
	//  (x) = (+1 +1 +0) (se)
	//  (y) = (+1 -1 +1) (up)
	phys_t x = phys_t{this->ne} * (+1) + this->se * (+1) + this->up * (+0);
	phys_t y = phys_t{this->ne} * (+1) + this->se * (-1) + this->up * (+1);

	// add scaling (w/2 for x, h/2 for y)
	return camgame_delta{
		static_cast<camgame_delta::elem_t>((x * (mgr.tile_size.x / 2)).to_int()),
		static_cast<camgame_delta::elem_t>((y * (mgr.tile_size.y / 2)).to_int())
	};
}


phys2_delta phys3_delta::to_phys2() const {
	return phys2_delta{this->ne, this->se};
}


double phys3_delta::length() const {
	return math::hypot3(this->ne.to_double(), this->se.to_double(), this->up.to_double());
}


phys3_delta phys3_delta::normalize(double length) const {
	return *this * (length / this->length());
}


camgame phys3::to_camgame(const CoordManager &mgr) const {
	return (*this - mgr.camgame_phys).to_camgame(mgr) + camgame{0, 0};
}


viewport phys3::to_viewport(const CoordManager &mgr) const {
	return this->to_camgame(mgr).to_viewport(mgr);
}


camhud phys3::to_camhud(const CoordManager &mgr) const {
	return this->to_viewport(mgr).to_camhud(mgr);
}


tile phys3::to_tile() const {
	return this->to_tile3().to_tile();
}


} // namespace openage::coord
