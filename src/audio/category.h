#ifndef _ENGINE_AUDIO_CATEGORY_H_
#define _ENGINE_AUDIO_CATEGORY_H_

#include "../../gamedata/sound_file.h"

namespace engine {
namespace audio {

enum class category_t {
	GAME,
	INTERFACE,
	MUSIC,
	TAUNT
};

category_t from_category(const gamedata::audio_category_t category);

}
}

#endif
