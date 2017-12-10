// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "pixel.h"

#include "coordmanager.h"
#include "phys.h"

namespace openage {
namespace coord {


phys3_delta camgame_delta::to_phys3(const CoordManager &mgr, phys_t up) const {
	// apply scaling factor; w/2 for x, h/2 for y
	phys_t x = phys_t(this->x) / static_cast<int64_t>(mgr.tile_halfsize.x);
	phys_t y = phys_t(this->y) / static_cast<int64_t>(mgr.tile_halfsize.y);

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


window camgame::to_window(const CoordManager &mgr) const {
	// reverse of window::to_camgame
	return (*this - camgame{0, 0}).to_window() + mgr.camgame_window;
}


phys3 window::to_phys3(const CoordManager &mgr, phys_t up) const {
    return this->to_camgame(mgr).to_phys3(mgr, up);
}


camhud camgame::to_camhud(const CoordManager &mgr) const {
    return this->to_window(mgr).to_camhud(mgr);
}


window_delta camgame_delta::to_window() const {
        return window_delta{this->x, -this->y};
}


phys3_delta window_delta::to_phys3(const CoordManager &mgr, phys_t up) const {
	return this->to_camgame().to_phys3(mgr, up);
}


camgame window::to_camgame(const CoordManager &mgr) const {
	// mgr.camgame_window describes the coordinates of camgame{0, 0} in the window system.
	return (*this - mgr.camgame_window).to_camgame() + camgame{0, 0};
}


tile window::to_tile(const CoordManager &mgr, phys_t up) const {
	return this->to_camgame(mgr).to_tile(mgr, up);
}


tile camgame::to_tile(const CoordManager &mgr, phys_t up) const {
	return this->to_phys3(mgr, up).to_tile();
}


camhud window::to_camhud(const CoordManager &mgr) const {
	return (*this - mgr.camhud_window).to_camhud() + camhud{0, 0};
}


window camhud::to_window(const CoordManager &mgr) const {
	// reverse of window::to_camhud
	return (*this - camhud{0, 0}).to_window() + mgr.camhud_window;
}


window_delta camhud_delta::to_window() const {
	return window_delta{this->x, -this->y};
}


}} // openage::coord
