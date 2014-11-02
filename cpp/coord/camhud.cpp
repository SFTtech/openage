// Copyright 2013-2014 the openage authors. See copying.md for legal info.

#define GEN_IMPL_CAMHUD_CPP
#include "camhud.h"

#include "term.h"
#include "window.h"
#include "../engine.h"
#include "../console/console.h"
#include "../util/misc.h"

namespace openage {
namespace coord {

window camhud::to_window() {
	Engine &e = Engine::get();
	return e.camhud_window + this->as_relative().to_window();
}

window_delta camhud_delta::to_window() const {
	//the direction of the y axis is flipped
	return window_delta {(pixel_t) x, (pixel_t) -y};
}

term camhud::to_term(console::Console *c) {
	term result;
	result.x = util::div<pixel_t>(x - c->bottomleft.x, c->charsize.x);
	result.y = util::div<pixel_t>(c->topright.y - y, c->charsize.x);
	return result;
}

} //namespace coord
} //namespace openage
