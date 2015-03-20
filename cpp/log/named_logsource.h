// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOG_NAMED_LOGSOURCE_H_
#define OPENAGE_LOG_NAMED_LOGSOURCE_H_

#include <string>

#include "logsource.h"

namespace openage {
namespace log {

class NamedLogSource : public LogSource {
public:
	NamedLogSource(const std::string &name);

	virtual std::string logsource_name();

private:
	std::string name;
};

}} // namespace openage::log

#endif
