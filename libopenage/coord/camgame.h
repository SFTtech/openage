// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_CAMGAME_H_
#define OPENAGE_COORD_CAMGAME_H_

#include "decl.h"
#include "xyz_coord.h"

namespace openage {
namespace coord {

struct camgame : public absolute_xyz_coord<camgame, camgame_delta, pixel_t, 2> {
	camgame() = default;
	camgame(pixel_t x, pixel_t y);

	phys3 to_phys3(phys_t up=0);
	window to_window();
};

struct camgame_delta : public relative_xyz_coord<camgame, camgame_delta, pixel_t, 2> {
	camgame_delta() = default;
	camgame_delta(pixel_t x, pixel_t y);

	phys3_delta to_phys3(phys_t up=0);
	window_delta to_window();
};

} // namespace coord
} // namespace openage

#undef MEMBERS

#endif
