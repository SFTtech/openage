// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <chrono>

#include "curve/curve.h"

namespace openage::event {

using simclock_t = std::chrono::steady_clock;
using timepoint_t = std::chrono::time_point<simclock_t>;

using dt_s_t = std::chrono::duration<long, std::ratio<1>>;
using dt_ms_t = std::chrono::duration<long, std::milli>;


enum class ClockState {
	INIT,
	STOPPED,
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
	 * Get the current simulation time in seconds.
     *
     * The returned value has a precision of milliseconds, so it is
     * accurate to three decimal places.
	 *
	 * @return Time passed (in seconds).
	 */
	curve::time_t get_time();

	/**
     * Set the speed of the clock.
     *
     * @param speed New speed of the clock.
     */
	void set_speed(util::FixedPoint<int64_t, 16> speed);

	/**
	 * Start the simulation timer.
	 */
	void start();

	/**
	 * Stop the simulation timer.
	 */
	void stop();

	/**
	 * Resume the simulation timer if the clock is stopped.
	 */
	void resume();

private:
	/**
     * Update the simulation time.
     */
	void update_time();

	/**
     * Status of the clock (init, running, stopped, ...).
     */
	ClockState state;

	/**
     * How fast time passes relative to real time.
     */
	util::FixedPoint<int64_t, 16> speed;

	/**
     * Last point in time where the clock was updated.
     */
	timepoint_t last_check;

	/**
     * Stores the time of the latest simulation iteration. It is updated whenever
	 * \p update_time() is called.
	 *
	 * The value essentially signifies how much time (in milliseconds) has passed
	 * _inside_ the simulation between starting the clock and the latest time check.
     */
	dt_ms_t sim_time;
};

} // namespace openage::event
