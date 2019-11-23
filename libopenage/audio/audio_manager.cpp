// Copyright 2014-2019 the openage authors. See copying.md for legal info.

#include "audio_manager.h"

#include <cstring>
#include <SDL2/SDL.h>
#include <sstream>

#include "error.h"
#include "hash_functions.h"
#include "resource.h"
#include "../log/log.h"
#include "../util/misc.h"


namespace openage {
namespace audio {




/**
 * Wrapper class for the sdl audio device locking so
 * the device doesn't deadlock because of funny exceptions.
 */
class SDLDeviceLock {
public:
	explicit SDLDeviceLock(const SDL_AudioDeviceID &id)
		:
		dev_id{id} {
		SDL_LockAudioDevice(this->dev_id);
	}

	~SDLDeviceLock() {
		SDL_UnlockAudioDevice(this->dev_id);
	}

	SDLDeviceLock(SDLDeviceLock &&) = delete;
	SDLDeviceLock(const SDLDeviceLock &) = delete;
	SDLDeviceLock& operator =(SDLDeviceLock &&) = delete;
	SDLDeviceLock& operator =(const SDLDeviceLock &) = delete;

private:
	SDL_AudioDeviceID dev_id;
};


AudioManager::AudioManager(job::JobManager *job_manager,
                           const std::string &device_name)
	:
	available{false},
	job_manager{job_manager},
	device_name{device_name} {

	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		log::log(MSG(err)
		         << "SDL audio initialization failed: "
		         << SDL_GetError());
		return;
	} else {
		log::log(MSG(info) << "SDL audio subsystems initialized");
	}

	// set desired audio output format
	SDL_AudioSpec desired_spec;
	SDL_zero(desired_spec);
	desired_spec.freq = 48000;
	desired_spec.format = AUDIO_S16LSB;
	desired_spec.channels = 2;
	desired_spec.samples = 4096;
	desired_spec.userdata = this;

	// call back that is invoked once SDL needs the next chunk of data
	desired_spec.callback = [] (void *userdata, uint8_t *stream, int len) {
		auto *audio_manager = static_cast<AudioManager*>(userdata);
		audio_manager->audio_callback(reinterpret_cast<int16_t*>(stream), len / 2);
	};

	// convert device name to valid parameter for sdl call
	// if the device name is empty, use a nullptr in order to indicate that the
	// default device should be used
	const char *c_device_name = device_name.empty() ?
	                            nullptr : device_name.c_str();
	// open audio playback device
	device_id = SDL_OpenAudioDevice(c_device_name, 0, &desired_spec,
	                                &device_spec, 0);

	// no device could be opened
	if (device_id == 0) {
		log::log(MSG(err) << "Error opening audio device: " << SDL_GetError());
		return;
	}

	// initialize playing sounds vectors
	using sound_vector = std::vector<std::shared_ptr<SoundImpl>>;
	playing_sounds.insert({category_t::GAME, sound_vector{}});
	playing_sounds.insert({category_t::INTERFACE, sound_vector{}});
	playing_sounds.insert({category_t::MUSIC, sound_vector{}});
	playing_sounds.insert({category_t::TAUNT, sound_vector{}});

	// create buffer for mixing
	this->mix_buffer = std::make_unique<int32_t[]>(
		4 * device_spec.samples * device_spec.channels
	);

	log::log(MSG(info) <<
	         "Using audio device: "
	         << (device_name.empty() ? "default" : device_name)
	         << " [freq=" << device_spec.freq
	         << ", format=" << device_spec.format
	         << ", channels=" << static_cast<uint16_t>(device_spec.channels)
	         << ", samples=" << device_spec.samples
	         << "]");

	SDL_PauseAudioDevice(device_id, 0);

	this->available = true;
}

AudioManager::~AudioManager() {
	SDL_CloseAudioDevice(device_id);
}

void AudioManager::load_resources(const std::vector<resource_def> &sound_files) {
	if (not this->available) {
		return;
	}

	for (auto &sound_file : sound_files) {
		auto key = std::make_tuple(sound_file.category, sound_file.id);

		if (this->resources.find(key) != std::end(this->resources)) {
			// sound is already loaded
			continue;
		}

		auto resource = Resource::create_resource(this, sound_file);
		this->resources.insert({key, resource});
	}
}

Sound AudioManager::get_sound(category_t category, int id) {
	if (not this->available) {
		throw error::Error{MSG(err) << "audio manager not available, "
		                            << "but sound was requested!"};
	}

	auto resource = resources.find(std::make_tuple(category, id));
	if (resource == std::end(resources)) {
		throw audio::Error{
			MSG(err) <<
			"Sound resource does not exist: "
			"category=" << category << ", " <<
			"id=" << id
		};
	}

	auto sound_impl = std::make_shared<SoundImpl>(resource->second);
	return Sound{this, sound_impl};
}


void AudioManager::audio_callback(int16_t *stream, int length) {
	std::memset(mix_buffer.get(), 0, length*4);

	// iterate over all categories
	for (auto &entry : this->playing_sounds) {
		auto &playing_list = entry.second;
		// iterate over all sounds in one category
		for (size_t i = 0; i < playing_list.size(); i++) {
			auto &sound = playing_list[i];
			auto sound_finished = sound->mix_audio(mix_buffer.get(), length);
			// if the sound is finished,
			// it should be removed from the playing list
			if (sound_finished) {
				util::vector_remove_swap_end(playing_list, i);
				i--;
			}
		}
	}

	// write the mix buffer to the output stream and adjust volume
	for (int i = 0; i < length; i++) {
		auto value = mix_buffer[i]/256;
		if (value > 32767) {
			value = 32767;
		} else if (value < -32768) {
			value = -32768;
		}
		stream[i] = static_cast<int16_t>(value);
	}
}

void AudioManager::add_sound(const std::shared_ptr<SoundImpl>& sound) {
	SDLDeviceLock lock{this->device_id};

	auto category = sound->get_category();
	auto &playing_list = this->playing_sounds.find(category)->second;
	// TODO probably check if sound already exists in playing list
	playing_list.push_back(sound);
}

void AudioManager::remove_sound(const std::shared_ptr<SoundImpl>& sound) {
	SDLDeviceLock lock{this->device_id};

	auto category = sound->get_category();
	auto &playing_list = this->playing_sounds.find(category)->second;

	for (size_t i = 0; i < playing_list.size(); i++) {
		if (playing_list[i] == sound) {
			util::vector_remove_swap_end(playing_list, i);
			break;
		}
	}
}

SDL_AudioSpec AudioManager::get_device_spec() const {
	return this->device_spec;
}

job::JobManager *AudioManager::get_job_manager() const {
	return this->job_manager;
}

bool AudioManager::is_available() const {
	return this->available;
}


std::vector<std::string> AudioManager::get_devices() {
	std::vector<std::string> device_list;
	auto num_devices = SDL_GetNumAudioDevices(0);
	device_list.reserve(num_devices);
	for (int i = 0; i < num_devices; i++) {
		device_list.emplace_back(SDL_GetAudioDeviceName(i, 0));
	}
	return device_list;
}

std::vector<std::string> AudioManager::get_drivers() {
	std::vector<std::string> driver_list;
	auto num_drivers = SDL_GetNumAudioDrivers();
	driver_list.reserve(num_drivers);
	for (int i = 0; i < num_drivers; i++) {
		driver_list.emplace_back(SDL_GetAudioDriver(i));
	}
	return driver_list;
}

std::string AudioManager::get_current_driver() {
	const char *c_driver = SDL_GetCurrentAudioDriver();
	if (c_driver == nullptr) {
		return "";
	} else {
		return c_driver;
	}
}

}} // openage::audio
