// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_TERM_H_
#define OPENAGE_COORD_TERM_H_

#include "decl.h"
#include "xyz_coord.h"

namespace openage {
namespace console {
class Console;
} // namespace console

namespace coord {

struct term : public absolute_xyz_coord< term, term_delta, term_t, 2  > {
	term() = default;
	term(term_t x, term_t y);

	camhud to_camhud(console::Console *e) const;
};

struct term_delta : public relative_xyz_coord<term, term_delta, term_t, 2 > {
	term_delta() = default;
	term_delta(term_t x, term_t y);

	term_t x, y;
};

} // namespace coord
} // namespace openage

#endif
