// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#include "loader_policy.h"

#include "../error/error.h"

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

}} // openage::audio
