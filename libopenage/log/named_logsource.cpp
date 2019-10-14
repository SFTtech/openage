// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "named_logsource.h"

#include <utility>

namespace openage::log {


NamedLogSource::NamedLogSource(std::string name)
	:
	name{std::move(name)} {}


std::string NamedLogSource::logsource_name() {
	return this->name;
}


NamedLogSource &general_source() {
	static NamedLogSource value{"general"};
	return value;
}


} // namespace openage::log
