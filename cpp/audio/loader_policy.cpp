#include "loader_policy.h"

#include "../log.h"
#include "../util/error.h"

namespace openage {
namespace audio {

loader_policy_t from_loader_policy(const gamedata::audio_loader_policy_t loader_policy) {
	switch (loader_policy) {
		case gamedata::audio_loader_policy_t::IN_MEMORY:
			return loader_policy_t::IN_MEMORY;
		case gamedata::audio_loader_policy_t::DYNAMIC:
			return loader_policy_t::DYNAMIC;
		default:
			throw util::Error{"Unknown loader policy: %d", static_cast<int>(loader_policy)};
	}
}

} //namespace audio
} //namespace openage
