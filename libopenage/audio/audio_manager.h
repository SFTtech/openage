// Copyright 2014-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <QObject>

#include "category.h"
#include "hash_functions.h"
#include "resource_def.h"
#include "sound.h"

Q_FORWARD_DECLARE_OBJC_CLASS(QAudioDevice);
Q_FORWARD_DECLARE_OBJC_CLASS(QAudioFormat);
Q_FORWARD_DECLARE_OBJC_CLASS(QAudioSink);

namespace openage {

namespace job {
class JobManager;
}


namespace audio {

/**
 * This class provides audio functionality for openage.
 *
 * TODO: Finish porting to Qt.
 */
class AudioManager {
public:
	/**
	 * Initializes the audio manager with the given device name.
	 * If the name is empty, the default device is used.
	 */
	AudioManager(const std::shared_ptr<job::JobManager> &job_manager,
	             const std::string &device_name = "");

	~AudioManager();

	AudioManager(const AudioManager &) = delete;
	AudioManager(AudioManager &&) = delete;

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
	const std::shared_ptr<QAudioFormat> &get_device_spec() const;

	/**
	 * Return the game engine the audio manager is attached to.
	 */
	const std::shared_ptr<job::JobManager> &get_job_manager() const;

	/**
	 * If this audio manager is available.
	 * It's not available if
	 */
	bool is_available() const;

	/**
	 * Returns a vector of all available device names.
	 */
	static std::vector<std::string> get_devices();

	/**
	 * Returns the default device name.
	 */
	static std::string get_default_device();

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
	std::shared_ptr<job::JobManager> job_manager;

	/**
	 * the used audio device's name
	 */
	std::string device_name;

	/**
	 * the audio output format
	 */
	std::shared_ptr<QAudioFormat> device_format;

	/**
	 * the used audio device's id
	 */
	std::shared_ptr<QAudioDevice> device;

	/**
	 * the audio sink
	 */
	std::shared_ptr<QAudioSink> audio_sink;

	/**
	 * Buffer used for mixing audio to one stream.
	 */
	std::unique_ptr<int32_t[]> mix_buffer;

	std::unordered_map<std::tuple<category_t, int>, std::shared_ptr<Resource>> resources;

	std::unordered_map<category_t, std::vector<std::shared_ptr<SoundImpl>>> playing_sounds;
};

} // namespace audio
} // namespace openage
