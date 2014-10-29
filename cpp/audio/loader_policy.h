#ifndef OPENAGE_AUDIO_LOADER_POLICY_H_
#define OPENAGE_AUDIO_LOADER_POLICY_H_

#include "../gamedata/sound_file.gen.h"

namespace openage {
namespace audio {

enum class loader_policy_t {
	IN_MEMORY,
	DYNAMIC
};

loader_policy_t from_loader_policy(const gamedata::audio_loader_policy_t loader_policy);

} //namespace audio
} //namespace openage

#endif
