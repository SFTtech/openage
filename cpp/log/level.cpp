// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "level.h"

namespace openage {

template<> log::level::data_type log::level::data{};

namespace log {

namespace lvl {

level MIN  {{-1000, "min loglevel", "5"}};
level spam {{ -100, "SPAM",         ""}};
level dbg  {{  -20, "DBG",          ""}};
level info {{    0, "INFO",         ""}};
level warn {{  100, "WARN",         "33"}};
level err  {{  200, "ERR",          "31;1"}};
level crit {{  500, "CRIT",         "31;1;47"}};
level MAX  {{ 1000, "MAX",          "5"}};

} // lvl


std::ostream &operator <<(std::ostream &os, level lvl) {
	os << lvl->name;
	return os;
}


}} // openage::log
