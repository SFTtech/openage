// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "simulation.h"

#include <mutex>

#include "event/clock.h"
#include "event/loop.h"
#include "event/state.h"
#include "log/log.h"

namespace openage::event {

Simulation::Simulation() :
	running{false},
	loop{std::make_shared<Loop>()},
	state{std::make_shared<State>(this->loop)},
	clock{std::make_shared<Clock>()} {}

Simulation::Simulation(const std::shared_ptr<Loop> loop,
                       const std::shared_ptr<Clock> clock) :
	running{false},
	loop{loop},
	state{std::make_shared<State>(this->loop)},
	clock{clock} {}

void Simulation::run() {
	this->start();
	while (this->running) {
		this->clock->update_time();
		this->loop->reach_time(this->clock->get_time(), this->state);
	}
	log::log(MSG(info) << "simulation loop exited");
}

void Simulation::start() {
	std::unique_lock lock{this->mutex};

	this->running = true;

	if (this->clock->get_state() == ClockState::INIT) {
		this->clock->start();
	}

	log::log(MSG(info) << "simulation started");
}

void Simulation::stop() {
	std::unique_lock lock{this->mutex};

	this->running = false;

	log::log(MSG(info) << "simulation stopped");
}

} // namespace openage::event
