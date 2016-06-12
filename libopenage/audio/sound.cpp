// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "sound.h"

#include <tuple>

#include "audio_manager.h"
#include "resource.h"

namespace openage {
namespace audio {


Sound::Sound(AudioManager *audio_manager, std::shared_ptr<SoundImpl> sound_impl)
	:
	audio_manager{audio_manager},
	sound_impl{sound_impl} {
}


category_t Sound::get_category() const {
	return sound_impl->get_category();
}


int Sound::get_id() const {
	return sound_impl->get_id();
}


void Sound::set_volume(int32_t volume) {
	sound_impl->volume = volume;
}


int32_t Sound::get_volume() const {
	return sound_impl->volume;
}


void Sound::set_looping(bool looping) {
	sound_impl->looping = looping;
}


bool Sound::is_looping() const {
	return sound_impl->looping;
}


void Sound::play() {
	if (!sound_impl->in_use) {
		sound_impl->resource->use();
		sound_impl->in_use = true;
	}
	sound_impl->offset = 0;
	if (!sound_impl->playing) {
		audio_manager->add_sound(sound_impl);
		sound_impl->playing = true;
	}
}


void Sound::pause() {
	if (sound_impl->playing) {
		audio_manager->remove_sound(sound_impl);
		sound_impl->playing = false;
	}
}


void Sound::resume() {
	if (!sound_impl->in_use) {
		sound_impl->resource->use();
		sound_impl->in_use = true;
	}
	if (!sound_impl->playing) {
		audio_manager->add_sound(sound_impl);
		sound_impl->playing = true;
	}
}


void Sound::stop() {
	sound_impl->offset = 0;
	if (sound_impl->playing) {
		audio_manager->remove_sound(sound_impl);
		sound_impl->playing = false;
	}
	if (sound_impl->in_use) {
		sound_impl->resource->stop_using();
		sound_impl->in_use = false;
	}
}


bool Sound::is_playing() const {
	return sound_impl->playing;
}


// here begins the internal sound implementation

SoundImpl::SoundImpl(std::shared_ptr<Resource> resource, int32_t volume)
	:
	resource{resource},
	in_use{false},
	volume{volume},
	offset{0},
	playing{false},
	looping{false} {
}


SoundImpl::~SoundImpl() {
	if (in_use) {
		resource->stop_using();
	}
}


category_t SoundImpl::get_category() const {
	return resource->get_category();
}


int SoundImpl::get_id() const {
	return resource->get_id();
}


bool SoundImpl::mix_audio(int32_t *stream, int length) {
	size_t stream_index = 0;
	while (length > 0) {
		auto chunk = resource->get_data(offset, length);

		if (chunk.length == 0) {
			if (looping) {
				offset = 0;
			} else {
				playing = false;
				return true;
			}
		} else if (chunk.data == nullptr) {
			return false;
		}

		for (size_t i = 0; i < chunk.length; i++) {
			stream[i+stream_index] += volume * chunk.data[i];
		}

		offset += chunk.length;
		length -= chunk.length;
		stream_index += chunk.length;
	}

	return false;
}


}} // namespace openage::audio
