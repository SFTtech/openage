#ifndef OPENAGE_AUDIO_LOADER_POLICY_H_6B9C73A0D663439FABF502B48D8E0C4A
#define OPENAGE_AUDIO_LOADER_POLICY_H_6B9C73A0D663439FABF502B48D8E0C4A

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
