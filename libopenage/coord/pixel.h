// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "coord_xy.gen.h"

namespace openage {
namespace coord {

using pixel_t = int32_t;


struct camgame_delta;
struct camgame;


struct camgame_delta : CoordXYRelative<pixel_t, camgame, camgame_delta> {
	using CoordXYRelative<pixel_t, camgame, camgame_delta>::CoordXYRelative;
};

struct camgame : CoordXYAbsolute<pixel_t, camgame, camgame_delta> {
	using CoordXYAbsolute<pixel_t, camgame, camgame_delta>::CoordXYAbsolute;
};


struct camhud_delta;
struct camhud;


struct camhud_delta : CoordXYRelative<pixel_t, camhud, camhud_delta> {
	using CoordXYRelative<pixel_t, camhud, camhud_delta>::CoordXYRelative;
};

struct camhud : CoordXYAbsolute<pixel_t, camhud, camhud_delta> {
	using CoordXYAbsolute<pixel_t, camhud, camhud_delta>::CoordXYAbsolute;
};


struct window_delta;
struct window;


struct window_delta : CoordXYRelative<pixel_t, window, window_delta> {
	using CoordXYRelative<pixel_t, window, window_delta>::CoordXYRelative;
};

struct window : CoordXYAbsolute<pixel_t, window, window_delta> {
	using CoordXYAbsolute<pixel_t, window, window_delta>::CoordXYAbsolute;
};


} // namespace coord
} // namespace openage
