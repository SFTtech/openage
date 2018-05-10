// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <ostream>


namespace openage {
namespace audio {

enum class format_t {
	OPUS,
	WAV,
	MP3,
	FLAC
};

const char *format_t_to_str(format_t val);

std::ostream &operator <<(std::ostream &os, format_t val);

}} // openage::audio
