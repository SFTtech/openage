// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOG_NAMED_LOGSOURCE_H_
#define OPENAGE_LOG_NAMED_LOGSOURCE_H_

// pxd: from libcpp.string cimport string
#include <string>

// pxd: from libopenage.log.logsource cimport LogSource
#include "logsource.h"

namespace openage {
namespace log {

/**
 * Simple pure log source class with a string name.
 *
 * pxd:
 *
 * cppclass NamedLogSource(LogSource):
 *     NamedLogSource(string name) except +
 */
class NamedLogSource : public LogSource {
public:
	NamedLogSource(const std::string &name);

	virtual std::string logsource_name();

private:
	std::string name;
};


/**
 * Returns a reference to a general named log source, for use by log::log().
 */
NamedLogSource &general_source();


}} // namespace openage::log

#endif
