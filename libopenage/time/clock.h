// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <chrono>
#include <shared_mutex>

#include "time/time.h"


namespace openage::time {

using simclock_t = std::chrono::steady_clock;
using timepoint_t = std::chrono::time_point<simclock_t>;

using speed_t = util::FixedPoint<int64_t, 16>;

using dt_s_t = std::chrono::duration<long, std::ratio<1>>;
using dt_ms_t = std::chrono::duration<long, std::milli>;


enum class ClockState {
	INIT,
	STOPPED,
	PAUSED,
	RUNNING
};

/**
 * Clock for timing simulation events.
 *
 * Time values have a precision of milliseconds which should
 * be accurate enough for all applications.
 */
class Clock {
public:
	Clock();
	~Clock() = default;

	/**
	 * Get the current state of the clock.
	 */
	ClockState get_state();

	/**
	 * Update the simulation time.
	 */
	void update_time();

	/**
	 * Get the current simulation time (in seconds).
	 *
	 * The returned value has a precision of milliseconds, so it is
	 * accurate to three decimal places.
	 *
	 * @return Time passed (in seconds).
	 */
	time::time_t get_time();

	/**
	 * Get the current simulation time without speed adjustments.
	 *
	 * The returned value has a precision of milliseconds, so it is
	 * accurate to three decimal places.
	 *
	 * @return Time passed (in seconds).
	 */
	time::time_t get_real_time();

	/**
	 * Get the current speed of the clock.
	 *
	 * @return Speed of the clock.
	 */
	speed_t get_speed();

	/**
	 * Set the speed of the clock.
	 *
	 * Simulation time is updated before changing speed.
	 *
	 * @param speed New speed of the clock.
	 */
	void set_speed(speed_t speed);

	/**
	 * Start the simulation timer.
	 */
	void start();

	/**
	 * Stop the simulation timer.
	 *
	 * Indicates that the simulation has ended.
	 */
	void stop();

	/**
	 * Pause the simulation timer.
	 *
	 * Simulation time is updated before stopping.
	 */
	void pause();

	/**
	 * Resume the simulation timer if the clock is stopped.
	 */
	void resume();

private:
	/**
	 * Status of the clock (init, running, stopped, ...).
	 */
	ClockState state;

	/**
	 * Maximum length of a simulation iteration (in milliseconds).
	 */
	uint16_t max_tick_time;

	/**
	 * How fast time passes relative to real time.
	 */
	speed_t speed;

	/**
	 * Last point in time where the clock was updated.
	 */
	timepoint_t last_check;

	/**
	 * Start of the simulation in real time.
	 */
	timepoint_t start_time;

	/**
	 * Stores how much time has passed inside the simulation (in milliseconds).
	 */
	time::time_t sim_time;

	/**
	 * Stores how much time has passed inside the simulation (in milliseconds)
	 * _without_ speed adjustments (i.e. it acts as if speed = 1.0).
	 */
	time::time_t sim_real_time;

	/**
	 * Mutex for protecting threaded access.
	 */
	std::shared_mutex mutex;
};

} // namespace openage::time
