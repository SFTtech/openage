// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "clock.h"

namespace openage::event {

Clock::Clock() :
	state{ClockState::INIT},
	speed{1.0f},
	last_check{simclock_t::now()},
	sim_time{dt_ms_t::zero()} {
}

void Clock::update_time() {
	if (this->state == ClockState::RUNNING) {
		auto now = simclock_t::now();
		dt_ms_t offset = std::chrono::duration_cast<std::chrono::milliseconds>(now - this->last_check);
		curve::time_t t = this->speed * offset.count();
		this->sim_time += dt_ms_t(t.to_int());
		this->last_check = now;
	}
}

curve::time_t Clock::get_time() {
	this->update_time();
	return this->sim_time.count();
}

void Clock::set_speed(util::FixedPoint<int64_t, 16> speed) {
	this->update_time();
	this->speed = speed;
}

void Clock::start() {
	this->last_check = simclock_t::now();
	this->state = ClockState::RUNNING;

	this->update_time();
}

void Clock::stop() {
	if (this->state == ClockState::RUNNING) {
		this->update_time();
	}

	this->state = ClockState::STOPPED;
}

void Clock::resume() {
	if (this->state == ClockState::STOPPED) [[likely]] {
		this->state = ClockState::RUNNING;
		this->update_time();
	}
}

} // namespace openage::event
