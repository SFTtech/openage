// Copyright 2021-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <shared_mutex>


namespace openage::time {
class Clock;

/**
 * Manages the passage of simulation time and real time.
 */
class TimeLoop {
public:
	/**
	 * Create a new time loop with default values.
	 */
	TimeLoop();

	/**
	 * Create a new time loop from an existing and clock.
	 */
	TimeLoop(const std::shared_ptr<Clock> clock);
	~TimeLoop() = default;

	/**
	 * Run the time loop.
	 *
	 * Updates the clock and dispatches events that happened.
	 */
	void run();

	/**
	 * Start the time loop.
	 */
	void start();

	/**
	 * Exit the time loop.
	 */
	void stop();

	/**
	 * Get the clock used by this time loop.
	 *
	 * @return Simulation clock.
	 */
	const std::shared_ptr<Clock> get_clock();

private:
	/**
	 * State of the time loop.
	 */
	bool running;

	/**
	 * Manage time and speed inside the simulation.
	 */
	std::shared_ptr<Clock> clock;

	/**
	 * Mutex for protecting threaded access.
	 */
	std::shared_mutex mutex;
};


} // namespace openage::time
