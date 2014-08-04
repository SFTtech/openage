#include "format.h"

#include "../util/error.h"

namespace openage {
namespace audio {

format_t from_format(const gamedata::audio_format_t format) {
	switch (format) {
		case gamedata::audio_format_t::OPUS:
			return format_t::OPUS;
		case gamedata::audio_format_t::WAV:
			return format_t::WAV;
		default:
			throw util::Error{"Unknown audio format: %d", static_cast<int>(format)};
	}
}

}
}
