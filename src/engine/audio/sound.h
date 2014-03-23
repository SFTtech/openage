#ifndef _ENGINE_AUDIO_SOUND_H_
#define _ENGINE_AUDIO_SOUND_H_

#include <memory>

#include "category.h"
#include "resource.h"

namespace engine {
namespace audio {

// forward declaration of AudioManager
class AudioManager;

class Sound {
private:
	std::shared_ptr<Resource> resource;

	int32_t volume;
	uint32_t position;

public:
	Sound(std::shared_ptr<Resource> resource, int32_t volume=128);
	~Sound() = default;

	category_t get_category() const;
	int get_id() const;

private:
	/*
	 * Mix this sound
	 */
	bool mix_audio(int32_t *stream, int len);

	friend class AudioManager;
};

}
}

#endif
