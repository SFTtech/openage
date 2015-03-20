// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#include "timer.h"

#include "../crossplatform/timing.h"

namespace openage {
namespace util {

Timer::Timer(bool stopped) {
	reset(stopped);
}

void Timer::reset(bool stopped) {
	this->stopped = stopped;
	if(stopped) {
		starttime = timing::get_monotonic_time();
	} else {
		stoppedat = 0;
	}
}

void Timer::stop() {
	if(!stopped) {
		stopped = true;
		stoppedat = timing::get_monotonic_time() - starttime;
	}
}

void Timer::start() {
	if(stopped) {
		stopped = false;
		starttime = timing::get_monotonic_time() - stoppedat;
	}
}

int64_t Timer::getval() const {
	if(stopped) {
		return stoppedat;
	} else {
		return timing::get_monotonic_time() - starttime;
	}
}

int64_t Timer::getandresetval() {
	int64_t result;
	if(stopped) {
		result = stoppedat;
		stoppedat = 0;
	} else {
		int64_t now = timing::get_monotonic_time();
		result = now - starttime;
		starttime = now;
	}

	return result;
}

bool Timer::isstopped() const {
	return stopped;
}

} //namespace util
} //namespace openage
