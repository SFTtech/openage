#ifndef OPENAGE_AUDIO_FORMAT_H_00F2F37FF0CF47D6AFE6D1BEEC800A32
#define OPENAGE_AUDIO_FORMAT_H_00F2F37FF0CF47D6AFE6D1BEEC800A32

#include "../gamedata/sound_file.gen.h"

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
