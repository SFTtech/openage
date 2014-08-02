#include "timer.h"

#include <SDL2/SDL.h>

namespace engine {
namespace util {

Timer::Timer() {
	reset(true);
}

void Timer::reset(bool stopped) {
	this->stopped = stopped;
	if(stopped) {
		starttime = SDL_GetTicks();
	} else {
		stoppedat = 0;
	}
}

void Timer::stop() {
	if(!stopped) {
		stopped = true;
		stoppedat = SDL_GetTicks() - starttime;
	}
}

void Timer::start() {
	if(stopped) {
		stopped = false;
		starttime = SDL_GetTicks() - stoppedat;
	}
}

unsigned Timer::getval() {
	if(stopped) {
		return stoppedat;
	} else {
		return SDL_GetTicks() - starttime;
	}
}

unsigned Timer::getandresetval() {
	unsigned result;
	if(stopped) {
		result = stoppedat;
		stoppedat = 0;
	} else {
		unsigned now = SDL_GetTicks();
		result = now - starttime;
		starttime = now;
	}

	return result;
}

bool Timer::isstopped() {
	return stopped;
}

} //namespace util
} //namespace engine
