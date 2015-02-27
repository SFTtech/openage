// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "level.h"

namespace openage {
namespace log {


level_properties get_level_properties(level lvl) {
	switch (lvl) {
	case level::MIN:  return {">min",     "5"};
	case level::spam: return {"SPAM",     ""};
	case level::dbg:  return {"DBG",      ""};
	case level::info: return {"INFO",     ""};
	case level::warn: return {"WARN",     "33"};
	case level::err:  return {"ERR",      "31;1"};
	case level::crit: return {"CRIT",     "31;1;47"};
	case level::MAX:  return {">max",     "5"};
	default:          return {">unknown", "5"};
	}
}


std::ostream &operator <<(std::ostream &os, level lvl) {
	level_properties lvl_properties = get_level_properties(lvl);
	os << lvl_properties.name;
	return os;
}


}} // namespace openage::log
