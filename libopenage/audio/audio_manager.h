// Copyright 2014-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>

#include "category.h"
#include "hash_functions.h"
#include "sound.h"
#include "resource_def.h"


namespace openage {

class Engine;

namespace job {
class JobManager;
}


namespace audio {

/**
 * This class provides audio functionality for openage.
 */
class AudioManager {
public:
	/**
	 * Initializes the audio manager with the given device name.
	 * If the name is empty, the default device is used.
	 */
	AudioManager(job::JobManager *job_manager,
	             const std::string &device_name="");

	~AudioManager();

	AudioManager(const AudioManager &) = delete;
	AudioManager(AudioManager &&)      = delete;

	AudioManager &operator=(const AudioManager &) = delete;
	AudioManager &operator=(AudioManager &&) = delete;

	/**
	 * Loads all audio resources, that are specified in the sound_files vector.
	 * @param sound_files a list of all sound resources
	 */
	void load_resources(const std::vector<resource_def> &sound_files);

	/**
	 * Returns a sound object with the given category and the given id. If no
	 * such sound exists an Error will be thrown.
	 * @param category the sound's category
	 * @param id the sound's id
	 */
	Sound get_sound(category_t category, int id);

	/**
	 * Called from the audio system once to request new data.
	 */
	void audio_callback(int16_t *stream, int length);

	/**
	 * Returns the currently used audio output format.
	 */
	SDL_AudioSpec get_device_spec() const;

	/**
	 * Return the game engine the audio manager is attached to.
	 */
	job::JobManager *get_job_manager() const;

	/**
	 * If this audio manager is available.
	 * It's not available if
	 */
	bool is_available() const;

private:
	void add_sound(const std::shared_ptr<SoundImpl> &sound);
	void remove_sound(const std::shared_ptr<SoundImpl> &sound);

	// Sound is the AudioManager's friend, so that only sounds can access the
	// add and remove sound method's
	friend class Sound;

	/**
	 * If no audio device was found, this audio manager
	 * will be unavailable.
	 */
	bool available;

	/**
	 * The job manager used in this audio manager for job queuing.
	 */
	job::JobManager *job_manager;

	/**
	 * the used audio device's name
	 */
	std::string device_name;

	/**
	 * the audio output format
	 */
	SDL_AudioSpec device_spec;

	/**
	 * the used audio device's id
	 */
	SDL_AudioDeviceID device_id;

	/**
	 * Buffer used for mixing audio to one stream.
	 */
	std::unique_ptr<int32_t[]> mix_buffer;

	std::unordered_map<std::tuple<category_t,int>,std::shared_ptr<Resource>> resources;

	std::unordered_map<category_t,std::vector<std::shared_ptr<SoundImpl>>> playing_sounds;

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

}} // openage::audio
