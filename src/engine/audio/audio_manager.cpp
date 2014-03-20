#include "audio_manager.h"

#include "../log.h"
#include "../util/error.h"

namespace engine {
namespace audio {

void global_audio_callback(void *userdata, Uint8 *stream, int len);

AudioManager::AudioManager(int freq, SDL_AudioFormat format, Uint8 channels,
		Uint16 samples)
		:
		AudioManager{"", freq, format, channels, samples} {
}

AudioManager::AudioManager(const std::string &device_name, int freq,
		SDL_AudioFormat format, Uint8 channels, Uint16 samples)
		:
		device_name{device_name} {
	//set desired audio output format
	SDL_AudioSpec desired_spec;
	SDL_zero(desired_spec);
	desired_spec.freq = freq;
	desired_spec.format = format;
	desired_spec.channels = channels;
	desired_spec.samples = samples;
	desired_spec.callback = global_audio_callback;

	// convert device name to valid parameter for sdl call
	// if the device name is empty, use a nullptr in order to indicate that the
	// default device should be used
	const char *c_device_name = device_name.empty() ? nullptr : device_name.c_str();
	device_id = SDL_OpenAudioDevice(c_device_name, 0, &desired_spec,
			&device_spec, 0);
	// no device could be opened
	if (device_id == 0) {
		throw Error{"Error opening audio device: %s", SDL_GetError()};	
	}

	log::msg("Using audio device %s [freq=%d,format=%d,channels=%d,samples=%d]",
			device_name.c_str(), device_spec.freq, device_spec.format,
			device_spec.channels, device_spec.samples);
}

AudioManager::~AudioManager() {
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

void global_audio_callback(void *userdata, Uint8 *stream, int len) {
	// TODO
}

}
}
