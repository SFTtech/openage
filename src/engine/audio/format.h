#ifndef _ENGINE_AUDIO_FORMAT_H_
#define _ENGINE_AUDIO_FORMAT_H_

#include "../../gamedata/sound_file.h"

namespace engine {
namespace audio {

enum class format_t {
	OPUS,
	WAV,
	MP3,
	FLAC
};

format_t from_format(const audio_format_t format);

}
}

#endif
