// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "clock.h"

namespace openage::event {

Clock::Clock() :
	state{ClockState::INIT},
	max_tick_time{50},
	speed{1.0f},
	last_check{simclock_t::now()},
	start_time{simclock_t::now()},
	sim_time{0},
	sim_real_time{0} {
}

void Clock::update_time() {
	if (this->state == ClockState::RUNNING) {
		std::unique_lock lock{this->mutex};

		auto now = simclock_t::now();
		auto passed = std::chrono::duration_cast<std::chrono::milliseconds>(now - this->last_check);
		if (passed.count() > this->max_tick_time) {
			// if too much real time passes between two time updates, we only advance time by a small amount
			// this prevents the simulation from getting out of control during unplanned stops,
			// e.g. when debugging or if you close your laptop lid
			this->sim_time += this->speed * this->max_tick_time;
			this->sim_real_time += this->max_tick_time;
		}
		else {
			this->sim_time += this->speed * passed.count();
			this->sim_real_time += passed.count();
		}
		this->last_check = now;
		// TODO: Stop clock if it reaches 0.0s with negative speed?
	}
}

curve::time_t Clock::get_time() {
	std::shared_lock lock{this->mutex};

	// convert time unit from milliseconds to seconds
	return this->sim_time / 1000;
}

curve::time_t Clock::get_real_time() {
	std::shared_lock lock{this->mutex};

	// convert time unit from milliseconds to seconds
	return this->sim_real_time / 1000;
}

void Clock::set_speed(speed_t speed) {
	std::unique_lock lock{this->mutex};

	this->update_time();
	this->speed = speed;
}

void Clock::start() {
	std::unique_lock lock{this->mutex};

	auto now = simclock_t::now();
	this->start_time = now;
	this->last_check = now;
	this->state = ClockState::RUNNING;
}

void Clock::stop() {
	std::unique_lock lock{this->mutex};

	if (this->state == ClockState::RUNNING) [[likely]] {
		this->update_time();
	}

	this->state = ClockState::STOPPED;
}

void Clock::resume() {
	std::unique_lock lock{this->mutex};

	if (this->state == ClockState::STOPPED) [[likely]] {
		this->last_check = simclock_t::now();
		this->state = ClockState::RUNNING;
	}
}

} // namespace openage::event
