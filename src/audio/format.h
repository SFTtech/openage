#ifndef _AUDIO_FORMAT_H_
#define _AUDIO_FORMAT_H_

#include "../gamedata/sound_file.h"

namespace openage {
namespace audio {

enum class format_t {
	OPUS,
	WAV,
	MP3,
	FLAC
};

format_t from_format(const gamedata::audio_format_t format);

}
}

#endif //_AUDIO_FORMAT_H_
