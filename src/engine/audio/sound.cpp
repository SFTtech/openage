#include "sound.h"

#include <tuple>

#include "audio_manager.h"
#include "../log.h"

namespace engine {
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
	sound_impl->position = 0;
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
	if (!sound_impl->playing) {
		audio_manager->add_sound(sound_impl);
		sound_impl->playing = true;
	}
}

void Sound::stop() {
	sound_impl->position = 0;
	if (sound_impl->playing) {
		audio_manager->remove_sound(sound_impl);
		sound_impl->playing = false;
	}
}

bool Sound::is_playing() const {
	return sound_impl->playing;
}

// here begins the internal sound implementation

SoundImpl::SoundImpl(std::shared_ptr<Resource> resource, int32_t volume)
		:
		resource{resource},
		volume{volume},
		position{0},
		playing{false},
		looping{false} {
	resource->use();
}

SoundImpl::~SoundImpl() {
	resource->stop_using();
}

category_t SoundImpl::get_category() const {
	return resource->get_category();
}

int SoundImpl::get_id() const {
	return resource->get_id();
}

bool SoundImpl::mix_audio(int32_t *stream, int len) {
	uint32_t stream_index = 0;
	while (len > 0) {
		const int16_t *samples;
		uint32_t num_samples;
		std::tie(samples, num_samples) = resource->get_samples(position, len);

		if (num_samples == 0) {
			if (looping) {
				position = 0;
				return false;
			} else {
				playing = false;
				return true;
			}
		} else if (samples == nullptr) {
			return false;
		}

		for (uint32_t i = 0; i < num_samples; i++) {
			stream[i+stream_index] += volume * samples[i];
		}

		position += num_samples;
		len -= num_samples;
		stream_index += num_samples;
	}

	return false;
}

}
}
