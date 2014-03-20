#ifndef _ENGINE_AUDIO_AUDIO_MANAGER_H_
#define _ENGINE_AUDIO_AUDIO_MANAGER_H_

#include <string>
#include <vector>

#include <SDL.h>

namespace engine {
namespace audio {

/*
 * This class provides audio functionality.
 */
class AudioManager {
public:
	/**
	 * Returns the currently used audio output format.
	 */
	SDL_AudioSpec get_device_spec() const;

	/**
	 * Returns a vector of all available device names.
	 */
	static std::vector<std::string> get_devices();

	/**
	 * Returns a vector of all available driver names.
	 */
	static std::vector<std::string> get_drivers();

	/**
	 * Returns the name of the currently used driver.
	 */
	static std::string get_current_driver();
};

}
}

#endif
