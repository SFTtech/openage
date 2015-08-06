// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "named_logsource.h"

namespace openage {
namespace log {


NamedLogSource::NamedLogSource(const std::string &name)
	:
	name{name} {}


std::string NamedLogSource::logsource_name() {
	return this->name;
}


NamedLogSource &general_source() {
	static NamedLogSource value{"general"};
	return value;
}


}} // namespace openage::log
