// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "clock.h"

namespace openage::event {

Clock::Clock() :
	state{ClockState::INIT},
	ref_time{simclock_t::now()},
	sim_time{dt_ms_t::zero()} {
}

curve::time_t Clock::get_time() {
	if (this->state == ClockState::RUNNING) {
		this->sim_time = std::chrono::duration_cast<std::chrono::milliseconds>(simclock_t::now() - this->ref_time);
	}

	return this->sim_time.count();
}

void Clock::start() {
	this->ref_time = simclock_t::now();
	this->sim_time = std::chrono::duration_cast<std::chrono::milliseconds>(simclock_t::now() - this->ref_time);
	this->state = ClockState::RUNNING;
}

void Clock::stop() {
	if (this->state == ClockState::RUNNING) {
		this->sim_time = std::chrono::duration_cast<std::chrono::milliseconds>(simclock_t::now() - this->ref_time);
	}

	this->state = ClockState::STOPPED;
}

void Clock::resume() {
	if (this->state == ClockState::STOPPED) [[likely]] {
		this->ref_time = simclock_t::now() - this->sim_time;
		this->state = ClockState::RUNNING;
	}
}

} // namespace openage::event
