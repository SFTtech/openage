#define GEN_IMPL_WINDOW_CPP
#include "window.h"

#include "camgame.h"
#include "camhud.h"
#include "../engine.h"

namespace openage {
namespace coord {

camgame window::to_camgame() {
	Engine &e = Engine::get();
	return (*this - e.camgame_window).to_camgame().as_absolute();
}

camgame_delta window_delta::to_camgame() {
	//the direction of the y axis is flipped
	return camgame_delta {(pixel_t) x, (pixel_t) -y};
}

camhud window::to_camhud() {
	Engine &e = Engine::get();
	return (*this - e.camhud_window).to_camhud().as_absolute();
}

camhud_delta window_delta::to_camhud() {
	//the direction of the y axis is flipped
	return camhud_delta {(pixel_t) x, (pixel_t) -y};
}

} //namespace coord
} //namespace openage
