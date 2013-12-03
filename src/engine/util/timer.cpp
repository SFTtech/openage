#include "timer.h"

#include <SDL2/SDL.h>

namespace engine {
namespace util {

void Timer::start() {
	this->starttime = SDL_GetTicks();
}

uint32_t Timer::getval() {
	uint32_t now = SDL_GetTicks();
	return now - this->starttime;
}

} //namespace util
} //namespace engine
