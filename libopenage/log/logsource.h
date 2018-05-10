// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp.string cimport string
#include <string>

// pxd: from libopenage.log.message cimport message
#include "message.h"


namespace openage {
namespace log {


/**
 * Any class that wants to provide .log() shall inherit from this.
 * Examples: Engine, Unit, ...
 *
 * pxd:
 *
 * cppclass LogSource:
 *     void log(message msg) except +
 *     const size_t logger_id
 *     string logsource_name() except +
 */
class OAAPI LogSource {
public:
	LogSource();

	// make class polymorphic to provide TypeInfo for dynamic casting.
	virtual ~LogSource() = default;

	/**
	 * Logs a message (get one via MSG(level)).
	 */
	void log(const message &msg);

	/**
	 * Initialized during the LogSource constructor,
	 * guaranteed to be unique.
	 */
	const size_t logger_id;

	virtual std::string logsource_name() = 0;

private:
	/**
	 * Provides unique logger ids.
	 */
	static size_t get_unique_logger_id();
};


}} // namespace openage::log
