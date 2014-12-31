// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_AUDIO_AUDIO_MANAGER_H_
#define OPENAGE_AUDIO_AUDIO_MANAGER_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>

#include "category.h"
#include "hash_functions.h"
#include "resource.h"
#include "sound.h"
#include "../util/dir.h"

#include "../gamedata/sound_file.gen.h"

namespace openage {
namespace audio {

/*
 * This class provides audio functionality.
 */
class AudioManager {
private:
	// the used audio device's name
	std::string device_name;

	// the audio output format
	SDL_AudioSpec device_spec;
	// the used audio device's id
	SDL_AudioDeviceID device_id;

	std::unique_ptr<int32_t[]> mix_buffer;

	std::unordered_map<std::tuple<category_t,int>,std::shared_ptr<Resource>>
			resources;

	std::unordered_map<category_t,std::vector<std::shared_ptr<SoundImpl>>>
			playing_sounds;

public:
	AudioManager();

	// pass empty device name to indicate, that the default device should be
	// used
	AudioManager(const std::string &device_name);

	~AudioManager();

	// delete copy and move constructors and operators
	AudioManager(const AudioManager&) = delete;
	AudioManager(AudioManager&&) = delete;

	AudioManager &operator=(const AudioManager&) = delete;
	AudioManager &operator=(AudioManager&&) = delete;

	/**
	 * Loads all audio resources, that are specified in the sound_files vector.
	 * @param sound_files a list of all sound resources
	 */
	void load_resources(const util::Dir &asset_dir, const std::vector<gamedata::sound_file> &sound_files);

	/**
	 * Returns a sound object with the given category and the given id. If no
	 * such sound exists an Error will be thrown.
	 * @param category the sound's category
	 * @param id the sound's id
	 */
	Sound get_sound(category_t category, int id);

	void audio_callback(int16_t *stream, int length);

	/**
	 * Returns the currently used audio output format.
	 */
	SDL_AudioSpec get_device_spec() const;

private:
	void add_sound(std::shared_ptr<SoundImpl> sound);
	void remove_sound(std::shared_ptr<SoundImpl> sound);

	// Sound is the AudioManager's friend, so that only sounds can access the
	// add and remove sound method's
	friend class Sound;

// static functions

public:
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
