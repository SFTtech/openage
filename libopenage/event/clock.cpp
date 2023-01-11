// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "clock.h"

namespace openage::event {

Clock::Clock() :
	state{ClockState::INIT},
	ref_time{simclock_t::now()},
	now{dt_ms_t::zero()} {
}

curve::time_t Clock::get_time() {
	if (state == ClockState::RUNNING) {
		this->now = std::chrono::duration_cast<std::chrono::milliseconds>(simclock_t::now() - this->ref_time);
	}

	return this->now.count();
}

void Clock::start() {
	this->ref_time = simclock_t::now();
	this->now = std::chrono::duration_cast<std::chrono::milliseconds>(simclock_t::now() - this->ref_time);
	this->state = ClockState::RUNNING;
}

void Clock::stop() {
	this->state = ClockState::STOPPED;
}

void Clock::resume() {
	if (this->state == ClockState::STOPPED) [[likely]] {
		this->ref_time = simclock_t::now() - this->now;
		this->state = ClockState::RUNNING;
	}
}

} // namespace openage::event
