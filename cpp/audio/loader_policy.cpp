// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "loader_policy.h"

#include "../util/error.h"

namespace openage {
namespace audio {


const char *loader_policy_t_to_str(loader_policy_t val) {
	switch (val) {
	case loader_policy_t::IN_MEMORY: return "IN_MEMORY";
	case loader_policy_t::DYNAMIC:   return "DYNAMIC";
	default:                         return "unknown";
	}
}


std::ostream &operator <<(std::ostream &os, const loader_policy_t val) {
	os << loader_policy_t_to_str(val);
	return os;
}


loader_policy_t from_loader_policy(const gamedata::audio_loader_policy_t loader_policy) {
	switch (loader_policy) {
	case gamedata::audio_loader_policy_t::IN_MEMORY:
		return loader_policy_t::IN_MEMORY;
	case gamedata::audio_loader_policy_t::DYNAMIC:
		return loader_policy_t::DYNAMIC;
	default:
		throw util::Error{MSG(err) << "Unknown loader policy: " << static_cast<int>(loader_policy)};
	}
}


} //namespace audio
} //namespace openage
