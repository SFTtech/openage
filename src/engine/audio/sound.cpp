#include "sound.h"

#include <tuple>

#include "../log.h"

namespace engine {
namespace audio {

Sound::Sound(std::shared_ptr<Resource> resource, int32_t volume)
		:
		resource{resource},
		volume{volume},
		position{0} {
}

category_t Sound::get_category() const {
	return resource->get_category();
}

int Sound::get_id() const {
	return resource->get_id();
}

bool Sound::mix_audio(int32_t *stream, int len) {
	const int16_t *samples;
	uint32_t num_samples;
	std::tie(samples, num_samples) = resource->get_samples(position, len);

	if (num_samples == 0) {
		position = 0;
		return false;
	} else if(samples == nullptr) {
		return false;
	}

	for (uint32_t i = 0; i < num_samples; i++) {
		stream[i] += volume * samples[i];
	}

	position += num_samples;
	return false;
}

}
}
