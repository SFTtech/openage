#ifndef _ENGINE_UTIL_TIMER_H_
#define _ENGINE_UTIL_TIMER_H_

#include <stdint.h>

namespace engine {
namespace util {

/**
Time measurement class.

Start it some time, and get the passed time since the start.
 */
class Timer {
public:
	Timer() : starttime(0) {} ;
	~Timer();

	/**
	stores the start time of this timer.

	this saves the number of milliseconds that passed since program launch.
	*/
	void start();

	/**
	measure the time passed since start.

	if start has not been called yet, it returns the time in milliseconds
	since SDL initialisation.

	@return time in milliseconds since start() call.
	*/
	uint32_t measure();

private:
	uint32_t starttime;
};

} //namespace util
} //namespace engine

#endif //_ENGINE_UTIL_TIMER_H_
