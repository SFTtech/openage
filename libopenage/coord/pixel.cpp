// Copyright 2016-2018 the openage authors. See copying.md for legal info.

#include "pixel.h"

#include "coordmanager.h"
#include "phys.h"

namespace openage {
namespace coord {


phys3_delta camgame_delta::to_phys3(const CoordManager &mgr, phys_t up) const {
	// apply scaling factor; w/2 for x, h/2 for y
	phys_t x = phys_t::from_int(this->x) / static_cast<int64_t>(mgr.tile_size.x / 2);
	phys_t y = phys_t::from_int(this->y) / static_cast<int64_t>(mgr.tile_size.y / 2);

	// apply transformation matrix to 'scaled',
	// to get the relative phys3 position
	//
	// a camgame position represents a line in the 3D phys space
	// a camgame delta of 0 might actually correlate to an arbitrarily
	// large phys delta.
	// we select one specific point on that line by explicitly specifying the
	// 'up' value of the result.
	//
	// the transformation matrix is:
	//
	//  (ne) = (+0.5 +0.5 +0.5) ( x)
	//  (se) = (+0.5 -0.5 -0.5) ( y)
	//  (up) = (+0.0 +0.0 +1.0) (up)
	phys3_delta result;
	result.ne = (x + y + up)/2L;
	result.se = (x - y - up)/2L;
	result.up = (        up);

	return result;
}


phys3 camgame::to_phys3(const CoordManager &mgr, phys_t up) const {
	return (*this - camgame{0, 0}).to_phys3(mgr, up) + mgr.camgame_phys;
}


viewport camgame::to_viewport(const CoordManager &mgr) const {
	// reverse of viewport::to_camgame
	return (*this - camgame{0, 0}).to_viewport() + mgr.camgame_viewport;
}


phys3 viewport::to_phys3(const CoordManager &mgr, phys_t up) const {
    return this->to_camgame(mgr).to_phys3(mgr, up);
}


camhud camgame::to_camhud(const CoordManager &mgr) const {
    return this->to_viewport(mgr).to_camhud(mgr);
}


viewport_delta camgame_delta::to_viewport() const {
        return viewport_delta{this->x, this->y};
}


phys3_delta viewport_delta::to_phys3(const CoordManager &mgr, phys_t up) const {
	return this->to_camgame().to_phys3(mgr, up);
}


camgame viewport::to_camgame(const CoordManager &mgr) const {
	// mgr.camgame_viewport describes the coordinates of camgame{0, 0} in the viewport system.
	return camgame{0, 0} + (*this - mgr.camgame_viewport).to_camgame();
}


tile viewport::to_tile(const CoordManager &mgr, phys_t up) const {
	return this->to_camgame(mgr).to_tile(mgr, up);
}


tile camgame::to_tile(const CoordManager &mgr, phys_t up) const {
	return this->to_phys3(mgr, up).to_tile();
}


camhud viewport::to_camhud(const CoordManager &mgr) const {
	return camhud{0, 0} + (*this - mgr.camhud_viewport).to_camhud();
}


viewport camhud::to_viewport(const CoordManager &mgr) const {
	// reverse of viewport::to_camhud
	return (*this - camhud{0, 0}).to_viewport() + mgr.camhud_viewport;
}


viewport_delta camhud_delta::to_viewport() const {
	return viewport_delta{this->x, this->y};
}


viewport_delta input_delta::to_viewport(const CoordManager &mgr) const {
	viewport_delta result;

	result.x = this->x;
	result.y = mgr.viewport_size.y - this->y;

	return result;
}


camgame_delta input_delta::to_camgame(const CoordManager &mgr) const {
	return this->to_viewport(mgr).to_camgame();
}


phys3_delta input_delta::to_phys3(const CoordManager &mgr, phys_t up) const {
	return this->to_viewport(mgr).to_camgame().to_phys3(mgr, up);
}


viewport input::to_viewport(const CoordManager &mgr) const {
	return viewport{0, 0} + (*this - input{0, 0}).to_viewport(mgr);
}


camgame input::to_camgame(const CoordManager &mgr) const {
	return this->to_viewport(mgr).to_camgame(mgr);
}


phys3 input::to_phys3(const CoordManager &mgr, phys_t up) const {
	return this->to_viewport(mgr).to_camgame(mgr).to_phys3(mgr, up);
}

}} // openage::coord
