// Copyright 2014-2023 the openage authors. See copying.md for legal info.

#include "audio_manager.h"

#include <cstring>
#include <sstream>

#include <QAudioDevice>
#include <QAudioFormat>
#include <QAudioSink>
#include <QMediaDevices>

#include "../log/log.h"
#include "../util/misc.h"
#include "error.h"
#include "hash_functions.h"
#include "resource.h"


namespace openage {
namespace audio {

AudioManager::AudioManager(const std::shared_ptr<job::JobManager> &job_manager,
                           const std::string &device_name) :
	available{false},
	job_manager{job_manager},
	device_name{device_name},
	device_format{std::make_shared<QAudioFormat>()},
	device{nullptr},
	audio_sink{nullptr} {
	// set desired audio output format
	this->device_format->setSampleRate(48000);
	this->device_format->setSampleFormat(QAudioFormat::SampleFormat::Int16);
	this->device_format->setChannelCount(2);

	// find the device with the given name
	for (auto &device : QMediaDevices::audioOutputs()) {
		if (device.description().toStdString() == device_name) {
			this->device = std::make_shared<QAudioDevice>(device);
			break;
		}
	}

	// select the default device if the name was not found
	if (not this->device) {
		this->device = std::make_shared<QAudioDevice>(QMediaDevices::defaultAudioOutput());
		return;
	}

	if (not this->device->isFormatSupported(*this->device_format)) {
		log::log(MSG(err) << "Audio device does not support the desired format!");
		return;
	}

	// create audio sink
	this->audio_sink = std::make_unique<QAudioSink>(*this->device.get(), *this->device_format.get());
	// TODO: connect callback to get audio data to device

	// initialize playing sounds vectors
	using sound_vector = std::vector<std::shared_ptr<SoundImpl>>;
	playing_sounds.insert({category_t::GAME, sound_vector{}});
	playing_sounds.insert({category_t::INTERFACE, sound_vector{}});
	playing_sounds.insert({category_t::MUSIC, sound_vector{}});
	playing_sounds.insert({category_t::TAUNT, sound_vector{}});

	// create buffer for mixing
	this->mix_buffer = std::make_unique<int32_t[]>(
		4 * device_format->bytesPerSample() * device_format->channelCount());

	log::log(MSG(info) << "Using audio device: "
	                   << (device_name.empty() ? "default" : device_name)
	                   << " [sample rate=" << device_format->sampleRate()
	                   << ", format=" << device_format->sampleFormat()
	                   << ", channels=" << device_format->channelCount()
	                   << ", samples=" << device_format->bytesPerSample()
	                   << "]");

	this->audio_sink->stop();

	this->available = true;
}

AudioManager::~AudioManager() {
	this->audio_sink->stop();
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
			MSG(err) << "Sound resource does not exist: "
						"category="
					 << category << ", "
					 << "id=" << id};
	}

	auto sound_impl = std::make_shared<SoundImpl>(resource->second);
	return Sound{this, sound_impl};
}


void AudioManager::audio_callback(int16_t *stream, int length) {
	std::memset(mix_buffer.get(), 0, length * 4);

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
		auto value = mix_buffer[i] / 256;
		if (value > 32767) {
			value = 32767;
		}
		else if (value < -32768) {
			value = -32768;
		}
		stream[i] = static_cast<int16_t>(value);
	}
}

void AudioManager::add_sound(const std::shared_ptr<SoundImpl> &sound) {
	auto category = sound->get_category();
	auto &playing_list = this->playing_sounds.find(category)->second;
	// TODO probably check if sound already exists in playing list
	playing_list.push_back(sound);
}

void AudioManager::remove_sound(const std::shared_ptr<SoundImpl> &sound) {
	auto category = sound->get_category();
	auto &playing_list = this->playing_sounds.find(category)->second;

	for (size_t i = 0; i < playing_list.size(); i++) {
		if (playing_list[i] == sound) {
			util::vector_remove_swap_end(playing_list, i);
			break;
		}
	}
}

const std::shared_ptr<QAudioFormat> &AudioManager::get_device_spec() const {
	return this->device_format;
}

const std::shared_ptr<job::JobManager> &AudioManager::get_job_manager() const {
	return this->job_manager;
}

bool AudioManager::is_available() const {
	return this->available;
}


std::vector<std::string> AudioManager::get_devices() {
	auto devices = QMediaDevices::audioOutputs();

	std::vector<std::string> device_list;
	device_list.reserve(devices.size());

	for (auto &device : devices) {
		device_list.emplace_back(device.description().toStdString());
	}

	return device_list;
}

std::string AudioManager::get_default_device() {
	return QMediaDevices::defaultAudioOutput().description().toStdString();
}

} // namespace audio
} // namespace openage
