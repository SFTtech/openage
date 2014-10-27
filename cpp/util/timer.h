#ifndef OPENAGE_ENGINE_UTIL_TIMER_H_2871BA927B394A699EF82EA10E83F388
#define OPENAGE_ENGINE_UTIL_TIMER_H_2871BA927B394A699EF82EA10E83F388

#include <stdint.h>

namespace openage {
namespace util {

/**
Time measurement class.

Start it some time, and get the passed time since the start.
 */
class Timer {
	bool stopped;
	union {
		//while paused, stores the current timer value
		uint32_t stoppedat;

		//while not paused, stores the time the timer has been started
		uint32_t starttime;
	};

public:
	Timer();

	void reset(bool stopped = true);
	void stop();
	void start();
	unsigned getval();
	unsigned getandresetval();
	bool isstopped();
};


} //namespace util
} //namespace openage

#endif //_ENGINE_UTIL_TIMER_H_
