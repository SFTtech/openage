#ifndef OPENAGE_AUDIO_CATEGORY_H_D91C13753D40412CA70E5460EA612FF7
#define OPENAGE_AUDIO_CATEGORY_H_D91C13753D40412CA70E5460EA612FF7

#include "../gamedata/sound_file.gen.h"

namespace openage {
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

#endif // _AUDIO_CATEGORY_H_
