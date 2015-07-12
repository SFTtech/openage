// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "format.h"

#include "../error/error.h"

namespace openage {
namespace audio {


format_t from_format(const gamedata::audio_format_t format) {
	switch (format) {
	case gamedata::audio_format_t::OPUS:
		return format_t::OPUS;
	case gamedata::audio_format_t::WAV:
		return format_t::WAV;
	default:
		throw Error{MSG(err) << "Unknown audio format: " << static_cast<int>(format)};
	}
}


const char *format_t_to_str(format_t val) {
	switch (val) {
	case format_t::OPUS: return "OPUS";
	case format_t::WAV:  return "WAV";
	case format_t::MP3:  return "MP3";
	case format_t::FLAC: return "FLAC";
	default:             return "unknown";
	}
}


std::ostream &operator <<(std::ostream &os, const format_t val) {
	os << format_t_to_str(val);
	return os;
}


}} // namespace openage::audio
