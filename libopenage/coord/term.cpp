// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#define GEN_IMPL_TERM_CPP
#include "term.h"

#include "camhud.h"
#include "../console/console.h"

namespace openage {
namespace coord {

term::term(term_t x, term_t y)
{
	this->x = x;
	this->y = y;
}

term_delta::term_delta(term_t x, term_t y)
{
	this->x = x;
	this->y = y;
}

camhud term::to_camhud(console::Console *c) const {
	camhud result;
	result.x = c->bottomleft.x + x * c->charsize.x;
	result.y = c->topright.y   - y * c->charsize.y;
	return result;
}

} // namespace coord
} // namespace openage
