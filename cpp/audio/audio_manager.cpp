// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "audio_manager.h"

#include <cstring>
#include <SDL2/SDL.h>
#include <sstream>

#include "resource.h"

#include "../log/log.h"
#include "../util/dir.h"
#include "../util/error.h"

namespace openage {
namespace audio {

void global_audio_callback(void *userdata, uint8_t *stream, int len);

AudioManager::AudioManager()
	:
	AudioManager{""} {
}

AudioManager::AudioManager(const std::string &device_name)
	:
	device_name{device_name} {

	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		throw util::Error(MSG(err) << "SDL audio initialization failed: " << SDL_GetError());
	} else {
		log::log(MSG(info) << "SDL audio subsystems initialized");
	}

	//set desired audio output format
	SDL_AudioSpec desired_spec;
	SDL_zero(desired_spec);
	desired_spec.freq = 48000;
	desired_spec.format = AUDIO_S16LSB;
	desired_spec.channels = 2;
	desired_spec.samples = 4096;
	desired_spec.callback = global_audio_callback;
	desired_spec.userdata = this;

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
		throw util::Error(MSG(err) << "Error opening audio device: " << SDL_GetError());
	}

	// initialize playing sounds vectors
	using sound_vector = std::vector<std::shared_ptr<SoundImpl>>;
	playing_sounds.insert({category_t::GAME, sound_vector{}});
	playing_sounds.insert({category_t::INTERFACE, sound_vector{}});
	playing_sounds.insert({category_t::MUSIC, sound_vector{}});
	playing_sounds.insert({category_t::TAUNT, sound_vector{}});

	mix_buffer.reset(new int32_t[4 * device_spec.samples *
			device_spec.channels]);

	log::log(MSG(info) <<
		"Using audio device: " << (device_name.empty() ? "default" : device_name) << " ["
		"freq=" << device_spec.freq << ", "
		"format=" << device_spec.format << ", "
		"channels=" << device_spec.channels << ", "
		"samples=" << device_spec.samples <<
		"]");

	SDL_PauseAudioDevice(device_id, 0);
}

AudioManager::~AudioManager() {
	SDL_CloseAudioDevice(device_id);
}

void AudioManager::load_resources(const util::Dir &asset_dir,
                                  const std::vector<gamedata::sound_file> &sound_files) {
	for (auto &sound_file : sound_files) {
		auto category = from_category(sound_file.category);
		auto id = sound_file.sound_id;

		std::string path = asset_dir.join(sound_file.path);
		auto format = from_format(sound_file.format);
		auto loader_policy = from_loader_policy(sound_file.loader_policy);

		auto key = std::make_tuple(category, id);
		auto resource = Resource::create_resource(category, id, path, format, loader_policy);

		// TODO check resource already existing
		resources.insert({key, resource});
	}
}

Sound AudioManager::get_sound(category_t category, int id) {
	auto resource = resources.find(std::make_tuple(category, id));
	if (resource == std::end(resources)) {
		throw util::Error{MSG(err) <<
			"Sound resource does not exist: "
			"category=" << category << ", " <<
			"id=" << id};
	}

	auto sound_impl = std::make_shared<SoundImpl>(resource->second);
	return Sound{this, sound_impl};
}

void remove_from_vector(std::vector<std::shared_ptr<SoundImpl>> &v, size_t i) {
	// current sound is the last in the list, so just remove it
	if (i == v.size()-1) {
		v.pop_back();
	// current sound is in the middle of the list, so it will be
	// exchanged with the last sound and the removed
	} else {
		v[i] = v.back();
		v.pop_back();
	}
}

void AudioManager::audio_callback(int16_t *stream, int length) {
	std::memset(mix_buffer.get(), 0, length*4);

	// iterate over all categories
	for (auto &entry : playing_sounds) {
		auto &playing_list = entry.second;
		// iterate over all sounds in one category
		for (size_t i = 0; i < playing_list.size(); i++) {
			auto &sound = playing_list[i];
			auto sound_finished = sound->mix_audio(mix_buffer.get(), length);
			// if the sound is finished, it should be removed from the playing
			// list
			if (sound_finished) {
				remove_from_vector(playing_list, i);
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

void AudioManager::add_sound(std::shared_ptr<SoundImpl> sound) {
	SDL_LockAudioDevice(device_id);

	auto category = sound->get_category();
	auto &playing_list = playing_sounds.find(category)->second;
	// TODO probably check if sound already exists in playing list
	playing_list.push_back(sound);

	SDL_UnlockAudioDevice(device_id);
}

void AudioManager::remove_sound(std::shared_ptr<SoundImpl> sound) {
	SDL_LockAudioDevice(device_id);

	auto category = sound->get_category();
	auto &playing_list = playing_sounds.find(category)->second;

	for (size_t i = 0; i < playing_list.size(); i++) {
		if (playing_list[i] == sound) {
			remove_from_vector(playing_list, i);
			break;
		}
	}

	SDL_UnlockAudioDevice(device_id);
}

SDL_AudioSpec AudioManager::get_device_spec() const {
	return device_spec;
}

std::vector<std::string> AudioManager::get_devices() {
	std::vector<std::string> device_list;
	auto num_devices = SDL_GetNumAudioDevices(0);
	for (int i = 0; i < num_devices; i++) {
		device_list.push_back(SDL_GetAudioDeviceName(i, 0));
	}
	return device_list;
}

std::vector<std::string> AudioManager::get_drivers() {
	std::vector<std::string> driver_list;
	auto num_drivers = SDL_GetNumAudioDrivers();
	for (int i = 0; i < num_drivers; i++) {
		driver_list.push_back(SDL_GetAudioDriver(i));
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

void global_audio_callback(void *userdata, uint8_t *stream, int len) {
	AudioManager *audio_manager = static_cast<AudioManager*>(userdata);
	audio_manager->audio_callback(reinterpret_cast<int16_t*>(stream), len / 2);
}

}
}
