// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <shared_mutex>

namespace openage {

namespace gamestate {
class GameState;
}

namespace event {

class Clock;
class Loop;

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

	void set_state(const std::shared_ptr<gamestate::GameState> state);

	/**
     * Get the clock used by this simulation.
     *
     * @return Simulation clock.
     */
	const std::shared_ptr<Clock> get_clock();

	/**
     * Get the event loop used by this simulation.
     *
     * @return Event loop.
     */
	const std::shared_ptr<Loop> get_loop();

private:
	/**
	 * State of the simulation loop.
	 */
	bool running;

	/**
     * Event loop for storing and dispatching simulation events.
     */
	std::shared_ptr<Loop> loop;

	/**
     * Manage time and speed inside the simulation.
     */
	std::shared_ptr<Clock> clock;

	/**
     * State of the game.
     */
	std::shared_ptr<gamestate::GameState> state;

	/**
	 * Mutex for protecting threaded access.
	 */
	std::shared_mutex mutex;
};


} // namespace event
} // namespace openage
