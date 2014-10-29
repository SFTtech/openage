#ifndef OPENAGE_AUDIO_SOUND_H_
#define OPENAGE_AUDIO_SOUND_H_

#include <memory>

#include "category.h"
#include "resource.h"

namespace openage {
namespace audio {

// forward declaration of AudioManager
class AudioManager;


/**
 * A SoundImpl is the internal implementation of a sound. It stores the sound's
 * complete state. It is passed around as a shared_ptr, such that the same
 * SoundImpl instance can be shared between the Sound objects themselves and the
 * AudioManager.
 */
class SoundImpl {
public:
	/**
	 * The shared audio resource, which provides the pcm data to play.
	 */
	std::shared_ptr<Resource> resource;
	/**
	 * Whether this sound currently actively uses it's shared audio resource.
	 */
	bool in_use;

	/**
	 * The sounds volume.
	 */
	int32_t volume;
	/**
	 * The sounds playing offset.
	 */
	uint32_t offset;

	/**
	 * Whether this sound is currently playing.
	 */
	bool playing;
	/**
	 * Whether this sound is currently looping.
	 */
	bool looping;

	SoundImpl(std::shared_ptr<Resource> resource, int32_t volume=128);
	~SoundImpl();

	/**
	 * Returns this sound's category.
	 */
	category_t get_category() const;
	/**
	 * Returns this sound's id.
	 */
	int get_id() const;

	/*
	 * Mix this sound with the given pcm stream and return whether it has
	 * finished or not.
	 * @param stream the stream to mix with
	 * @param len the number of values that should mixed
	 */
	bool mix_audio(int32_t *stream, int len);

};

/**
 * A sound object is the direct interface to the user. It provides methods to
 * control the sound, e.g. play, stop, etc. It is a lightweight object that
 * stores a shared_ptr to its internal SoundImpl and a pointer to its
 * AudioManager.
 */
class Sound {
private:
	/**
	 * The audio manager that manages this sound.
	 */
	AudioManager *audio_manager;
	/**
	 * The internal sound implementation. This is where the internal state, e.g.
	 * current playing offset, volume, etc., is stored.
	 */
	std::shared_ptr<SoundImpl> sound_impl;

public:
	/**
	 * Returns this sound's category.
	 */
	category_t get_category() const;
	/**
	 * Returns this sound's id.
	 */
	int get_id() const;

	/**
	 * Sets this sound's volume. The volume should be a value in range [0,256],
	 * where 0 means silence and 256 the original pcm volume. A volume greater
	 * than 256 could lead to overmodulation.
	 * @param volume the new volume
	 */
	void set_volume(int32_t volume);
	/**
	 * Returns this sound's volume.
	 */
	int32_t get_volume() const;

	/**
	 * Sets whether this sound should be looping or not. A looping sound
	 * restarts automatically after it has finishes.
	 * @param looping true, if this sound should be looping, otherwise false
	 */
	void set_looping(bool looping);	
	/**
	 * Returns whether this sound is looping.
	 */
	bool is_looping() const;

	/**
	 * Resets the sound to it's beginning and starts playing it.
	 */
	void play();
	/**
	 * Pauses the sound at it's current playing offset.
	 */
	void pause();
	/**
	 * Resumes the sound at it's current playing offset.
	 */
	void resume();
	/**
	 * Resets the sound to it's beginning and stops playing it.
	 */
	void stop();

	/**
	 * Returns whether this sound is currently playing.
	 */
	bool is_playing() const;

private:
	Sound(AudioManager *audio_manager, std::shared_ptr<SoundImpl> sound_impl);

	friend class AudioManager;
};

}
}

#endif
