#include "category.h"

#include "../log.h"
#include "../util/error.h"

namespace engine {
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
			throw Error{"Unknown audio category: %d",
					static_cast<int>(category)};
	}
}

}
}
