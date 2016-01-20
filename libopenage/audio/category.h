// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>

#include "../gamedata/sound_file.gen.h"

namespace openage {
namespace audio {


enum class category_t {
	GAME,
	INTERFACE,
	MUSIC,
	TAUNT
};


const char *category_t_to_str(category_t val);
std::ostream &operator <<(std::ostream &os, category_t val);


category_t from_category(const gamedata::audio_category_t category);


}} // namespace openage::audio
