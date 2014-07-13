#ifndef _ENGINE_AUDIO_LOADER_POLICY_H_
#define _ENGINE_AUDIO_LOADER_POLICY_H_

#include "../../gamedata/sound_file.h"

namespace engine {
namespace audio {

enum class loader_policy_t {
	IN_MEMORY,
	DYNAMIC
};

loader_policy_t from_loader_policy(const gamedata::audio_loader_policy_t loader_policy);

}
}

#endif
