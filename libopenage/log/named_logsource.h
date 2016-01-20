// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

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

	std::string logsource_name() override;

private:
	std::string name;
};


/**
 * Returns a reference to a general named log source, for use by log::log().
 */
NamedLogSource &general_source();


}} // namespace openage::log
