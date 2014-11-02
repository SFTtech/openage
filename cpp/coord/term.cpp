// Copyright 2013-2014 the openage authors. See copying.md for legal info.

#define GEN_IMPL_TERM_CPP
#include "term.h"

#include "camhud.h"
#include "../console/console.h"

namespace openage {
namespace coord {

camhud term::to_camhud(console::Console *c) const {
	camhud result;
	result.x = c->bottomleft.x + x * c->charsize.x;
	result.y = c->topright.y   - y * c->charsize.y;
	return result;
}

} //namespace coord
} //namespace openage
