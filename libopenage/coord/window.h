// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_WINDOW_H_
#define OPENAGE_COORD_WINDOW_H_

#include "decl.h"

#include "xyz_coord.h"

namespace openage {
namespace coord {

struct window : public absolute_xyz_coord<window, window_delta, pixel_t, 2> {
	window() = default;
	window(pixel_t x, pixel_t y);

	camgame to_camgame() const;
	camhud to_camhud() const;
};

struct window_delta : public relative_xyz_coord<window, window_delta, pixel_t, 2> {
	window_delta() = default;
	window_delta(pixel_t x, pixel_t y);

	camgame_delta to_camgame() const;
	camhud_delta to_camhud() const;
};


} // namespace coord
} // namespace openage


#endif
