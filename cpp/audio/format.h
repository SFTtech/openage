// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_AUDIO_FORMAT_H_
#define OPENAGE_AUDIO_FORMAT_H_

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

#endif
