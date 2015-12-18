// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_CAMHUD_H_
#define OPENAGE_COORD_CAMHUD_H_

#include "decl.h"

#include "xyz_coord.h"

namespace openage {
namespace console {
class Console;
} // namespace console

namespace coord {

struct camhud : public absolute_xyz_coord<camhud, camhud_delta, pixel_t, 2 > {
	camhud() = default;
	camhud(pixel_t x, pixel_t y);

	window to_window();
	term to_term(console::Console *c);
};

struct camhud_delta : public relative_xyz_coord<camhud, camhud_delta, pixel_t, 2 > {
	camhud_delta() = default;
	camhud_delta(pixel_t x, pixel_t y);

	window_delta to_window() const;
};

} // namespace coord
} // namespace openage

#endif
