// Copyright 2013-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>

#include "timing.h"


namespace openage {
namespace util {

/**
 * Time measurement class.
 */
class Timer {
	bool stopped;
	union {
		/**
		 * while paused, stores the current timer value
		 */
		time_nsec_t stoppedat;

		/**
		 * while running, stores the time the timer is counting from
		 */
		time_nsec_t starttime;
	};

public:
	/**
	 * creates the timer, in either stopped or running state.
	 */
	Timer(bool stopped=true);

	/**
	 * resets the timer, in either stopped or running state.
	 */
	void reset(bool stopped=true);

	/**
	 * stops/pauses the timer.
	 */
	void stop();

	/**
	 * starts/unpauses the timer.
	 */
	void start();

	/**
	 * reads the current timer value, in nanoseconds.
	 */
	time_nsec_t getval() const;

	/**
	 * reads the current timer value, in nanoseconds,
	 * and resets the timer to zero (preserving started/stopped state).
	 */
	time_nsec_t getandresetval();

	/**
	 * returns whether the timer is currently running.
	 */
	bool isstopped() const;
};


}} // openage::util
