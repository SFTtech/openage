// Copyright 2013-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_WINDOW_H_
#define OPENAGE_COORD_WINDOW_H_

#include "decl.h"

#define MEMBERS x, y
#define SCALAR_TYPE pixel_t
#define ABSOLUTE_TYPE window
#define RELATIVE_TYPE window_delta

namespace openage {
namespace coord {

struct window {
	pixel_t x, y;

	#include "ops/abs.h"

	camgame to_camgame() const;
	camhud to_camhud() const;
};

struct window_delta {
	pixel_t x, y;

	#include "ops/rel.h"

	camgame_delta to_camgame() const;
	camhud_delta to_camhud() const;
};

#include "ops/free.h"

#ifdef GEN_IMPL_WINDOW_CPP
#include "ops/impl.h"
#endif //GEN_IMPL_WINDOW_CPP

} // namespace coord
} // namespace openage

#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE

#endif
