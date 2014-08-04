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
	return camhud_window + this->as_relative().to_window();
}

window_delta camhud_delta::to_window() {
	//the direction of the y axis is flipped
	return window_delta {(pixel_t) x, (pixel_t) -y};
}

term camhud::to_term() {
	term result;
	result.x = util::div<pixel_t>(x - console::bottomleft.x, console::charsize.x);
	result.y = util::div<pixel_t>(console::topright.y - y, console::charsize.x);
	return result;
}

} //namespace coord
} //namespace openage
