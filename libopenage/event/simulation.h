// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <shared_mutex>

namespace openage::event {

class Clock;
class Loop;
class State;

/**
 * Manage event loop and clock.
 */
class Simulation {
public:
	/**
     * Create a new simulation with default values.
     */
	Simulation();

	/**
     * Create a new simulation from an existing event loop and clock.
     */
	Simulation(const std::shared_ptr<Loop> loop,
	           const std::shared_ptr<Clock> clock);

	~Simulation() = default;

	/**
	 * Run the simulation loop.
     * 
     * Updates the clock and dispatches events that happened.
	 */
	void run();

	/**
	 * Start the simulation.
	 */
	void start();

	/**
	 * Exit the simulation loop.
	 */
	void stop();

	/**
     * Get the clock used by this simulation.
     * 
     * @return Simulation clock.
     */
	std::shared_ptr<Clock> get_clock() const;

	/**
     * Get the event loop used by this simulation.
     * 
     * @return Event loop.
     */
	std::shared_ptr<Clock> get_loop() const;

private:
	/**
	 * current engine state variable.
	 * set to false to stop the simulation loop.
	 */
	bool running;

	/**
     * Event loop for storing and dispatching simulation events.
     */
	std::shared_ptr<Loop> loop;

	/**
     * State.
     */
	std::shared_ptr<State> state;

	/**
     * Manage time and speed inside the simulation.
     */
	std::shared_ptr<Clock> clock;

	/**
	 * Mutex for protecting threaded access.
	 */
	std::shared_mutex mutex;
};


} // namespace openage::event