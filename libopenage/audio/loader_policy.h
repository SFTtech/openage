// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#pragma once


#include <ostream>


namespace openage {
namespace audio {

enum class loader_policy_t {
	IN_MEMORY,
	DYNAMIC
};


const char *loader_policy_t_to_str(loader_policy_t val);

std::ostream &operator <<(std::ostream &os, loader_policy_t val);

}} // openage::audio
