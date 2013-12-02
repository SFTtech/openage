#define GEN_IMPL_CAMHUD_CPP
#include "camhud.h"

#include "window.h"
#include "../engine.h"

namespace openage {
namespace engine {
namespace coord {

window camhud::to_window() {
	return camhud_window + this->as_relative().to_window();
}

window_delta camhud_delta::to_window() {
	//the direction of the y axis is flipped
	return window_delta {(pixel_t) x, (pixel_t) -y};
}

} //namespace coord
} //namespace engine
} //namespace openage
