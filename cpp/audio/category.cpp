// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "category.h"

#include "../log/log.h"
#include "../error/error.h"
#include "../util/strings.h"

namespace openage {
namespace audio {


category_t from_category(const gamedata::audio_category_t category) {
	switch (category) {
	case gamedata::audio_category_t::GAME:
		return category_t::GAME;
	case gamedata::audio_category_t::INTERFACE:
		return category_t::INTERFACE;
	case gamedata::audio_category_t::MUSIC:
		return category_t::MUSIC;
	case gamedata::audio_category_t::TAUNT:
		return category_t::TAUNT;
	default:
		throw Error{MSG(err) << "Unknown audio category: " << static_cast<int>(category)};
	}
}


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


} // namespace audio
} // namespace openage
