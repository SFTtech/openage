// Copyright 2013-2016 the openage authors. See copying.md for legal info.

#include "timer.h"

#include "timing.h"

namespace openage {
namespace util {

Timer::Timer(bool stopped) {
	this->reset(stopped);
}

void Timer::reset(bool stopped) {
	this->stopped = stopped;
	if (this->stopped) {
		this->stoppedat = 0;
	} else {
		this->starttime = timing::get_monotonic_time();
	}
}

void Timer::stop() {
	if (not stopped) {
		this->stopped = true;
		this->stoppedat = timing::get_monotonic_time() - this->starttime;
	}
}

void Timer::start() {
	if (this->stopped) {
		this->stopped = false;
		this->starttime = timing::get_monotonic_time() - this->stoppedat;
	}
}

time_nsec_t Timer::getval() const {
	if (this->stopped) {
		return this->stoppedat;
	} else {
		return timing::get_monotonic_time() - this->starttime;
	}
}

time_nsec_t Timer::getandresetval() {
	time_nsec_t result;

	if (this->stopped) {
		result = this->stoppedat;
		this->stoppedat = 0;
	}
	else {
		time_nsec_t now = timing::get_monotonic_time();
		result = now - this->starttime;
		this->starttime = now;
	}

	return result;
}

bool Timer::isstopped() const {
	return this->stopped;
}

}}  // openage::util
