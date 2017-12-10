// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "coord_xy.gen.h"
#include "declarations.h"

namespace openage {
namespace coord {


struct camgame_delta : CoordXYRelative<pixel_t, camgame, camgame_delta> {
	using CoordXYRelative<pixel_t, camgame, camgame_delta>::CoordXYRelative;

	/**
	 * There are infinite solutions to this conversion problem because
	 * a 2D coordinate is converted into a 3D coordinate.
	 *
	 * The user needs to manually give the 'up' value of the phys3 result.
	 */
	phys3_delta to_phys3(const CoordManager &mgr, phys_t up=phys_t::zero) const;
	window_delta to_window() const;
};


struct camgame : CoordXYAbsolute<pixel_t, camgame, camgame_delta> {
	using CoordXYAbsolute<pixel_t, camgame, camgame_delta>::CoordXYAbsolute;

	/**
	 * See the comments for camgame_delta::to_phys3.
	 *
	 * TODO: Once we have terrain elevation, 'up' will not mean the absolute
	 *       elevation, but instead the returned phys3 coordinate will be
	 *       the intersection between the camgame line and the 3d terrain +
	 *       up altitude.
	 */
	window to_window(const CoordManager &mgr) const;
	camhud to_camhud(const CoordManager &mgr) const;
	phys3 to_phys3(const CoordManager &mgr, phys_t up=phys_t::zero) const;
	tile to_tile(const CoordManager &mgr, phys_t up=phys_t::zero) const;
};


struct camhud_delta : CoordXYRelative<pixel_t, camhud, camhud_delta> {
	using CoordXYRelative<pixel_t, camhud, camhud_delta>::CoordXYRelative;
	window_delta to_window() const;
};


struct camhud : CoordXYAbsolute<pixel_t, camhud, camhud_delta> {
	using CoordXYAbsolute<pixel_t, camhud, camhud_delta>::CoordXYAbsolute;

	window to_window(const CoordManager &mgr) const;
};


struct window_delta : CoordXYRelative<pixel_t, window, window_delta> {
	using CoordXYRelative<pixel_t, window, window_delta>::CoordXYRelative;

	constexpr camgame_delta to_camgame() const {
		return camgame_delta{this->x, -this->y};
	}

	phys3_delta to_phys3(const CoordManager &mgr, phys_t up) const;
	camhud_delta to_camhud() const {
		return camhud_delta{this->x, -this->y};
	}
};


struct window : CoordXYAbsolute<pixel_t, window, window_delta> {
	using CoordXYAbsolute<pixel_t, window, window_delta>::CoordXYAbsolute;

	camgame to_camgame(const CoordManager &mgr) const;
	phys3 to_phys3(const CoordManager &mgr, phys_t up=phys_t::zero) const;
	tile to_tile(const CoordManager &mgr, phys_t up=phys_t::zero) const;
	camhud to_camhud(const CoordManager &mgr) const;
};


}} // openage::coord
