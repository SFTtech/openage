// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#include "category.h"

#include "../log/log.h"
#include "../error/error.h"
#include "../util/strings.h"

namespace openage {
namespace audio {


const char *category_t_to_str(category_t val) {
	switch (val) {
	case category_t::GAME:      return "GAME";
	case category_t::INTERFACE: return "INTERFACE";
	case category_t::MUSIC:     return "MUSIC";
	case category_t::TAUNT:     return "TAUNT";
	default:                    return "unknown";
	}
}


std::ostream &operator <<(std::ostream &os, const category_t val) {
	os << category_t_to_str(val);
	return os;
}


}} // openage::audio
