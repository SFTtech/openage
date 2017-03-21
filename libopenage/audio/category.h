// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>

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


}} // openage::audio
