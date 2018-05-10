// Copyright 2013-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "decl.h"

#define MEMBERS x, y
#define SCALAR_TYPE pixel_t
#define ABSOLUTE_TYPE camhud
#define RELATIVE_TYPE camhud_delta

namespace openage {
namespace console {
class Console;
} // namespace console

namespace coord {

struct camhud {
	pixel_t x, y;

	#include "ops/abs.h"

	window to_window();
	term to_term(console::Console *c);
};

struct camhud_delta {
	pixel_t x, y;

	#include "ops/rel.h"

	window_delta to_window() const;
};

#include "ops/free.h"

#ifdef GEN_IMPL_CAMHUD_CPP
#include "ops/impl.h"
#endif //GEN_IMPL_CAMHUD_CPP

} // namespace coord
} // namespace openage

#undef MEMBERS
#undef RELATIVE_TYPE
#undef ABSOLUTE_TYPE
#undef SCALAR_TYPE
