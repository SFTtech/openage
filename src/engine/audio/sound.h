#ifndef _ENGINE_AUDIO_SOUND_H_
#define _ENGINE_AUDIO_SOUND_H_

#include <memory>

#include "category.h"
#include "resource.h"

namespace engine {
namespace audio {

// forward declaration of AudioManager
class AudioManager;

class SoundImpl {
public:
	std::shared_ptr<Resource> resource;

	int32_t volume;
	uint32_t position;

	bool playing;
	bool looping;

	~SoundImpl() = default;

	category_t get_category() const;
	int get_id() const;

	SoundImpl(std::shared_ptr<Resource> resource, int32_t volume=128);

	/*
	 * Mix this sound and return whether it has finished or not.
	 */
	bool mix_audio(int32_t *stream, int len);

};

class Sound {
private:
	AudioManager *audio_manager;
	std::shared_ptr<SoundImpl> sound_impl;

public:
	category_t get_category() const;
	int get_id() const;

	void set_volume(int32_t volume);
	int32_t get_volume() const;

	void set_looping(bool looping);	
	bool is_looping() const;

	void play();
	void pause();
	void resume();
	void stop();

	bool is_playing() const;

private:
	Sound(AudioManager *audio_manager, std::shared_ptr<SoundImpl> sound_impl);

	friend class AudioManager;
};

}
}

#endif
