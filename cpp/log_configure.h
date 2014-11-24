// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOG_CONFIGURE_H_
#define OPENAGE_LOG_CONFIGURE_H_

namespace openage { namespace log {
	// Policy based logger, use noop_printer to disable the associated information
	// Once a logger has been defined, don't forget to register it through the specialization 
	// of configure<LogManager>::logger_list (see below).

	using ConsoleLogger = AsyncLogger<
			  cout
			, time_printer
			, noop_printer
			, noop_printer
			, colored_loglevel_printer
			, origin_printer
	>;

	/* Some examples of log configuration */
	/*
	OPENAGE_TPL_STRING(openage_log, "openage.log");
	using FileLogger = AsyncLogger<
			  fout<openage_log>
			, datetime_printer
			, tag_printer
			, thread_printer
			, loglevel_printer   // txt editors wont support colors
			, origin_printer
			, type_list<>
			, type_list<YourMom> // we don't want YourMom there
	>;

	// YourMom deserves its own log file
	// Whenever we write something like :
	// 	log::tmsg(YourMom, "Don't forget to close the door.")
	// this will push a new log entry into this logger.
	// All others log tags will be ignored by this logger.
	OPENAGE_TPL_STRING(yourmom_log, "yourmom.log");
	using YourMomFileLogger = AsyncLogger<
			  fout<yourmom_log>
			, datetime_printer
			, noop_printer      // we don't need to print the tags as YourMom is the only one there
			, thread_printer
			, loglevel_printer
			, origin_printer
			, type_list<YourMom>  // only accepts YourMom logger tag!
	>;
	*/

	// This specialization of configure for the LogManager has to define the logger list used throughout the whole application.
	// This logger list has to be maintained each time a new logger is defined.
	template <>
	struct configure<LogManager> {
		using logger_list = type_list<ConsoleLogger>;
	};

	// Defines the default logger tag verbosity level
	template <>
	struct configure<root> {
		static const constexpr LogLevel log_level = LogLevel::MSG;
	};

	/* Example of YourMom log configuration */
	/*
	// Defines YourMom logger tag verbosity level
	template <>
	struct configure<YourMom> {
		// You should always listen to YourMom but she's just talking too much
		static const constexpr LogLevel log_level = LogLevel::IMP;
	};
	*/
}} // namespace openage::log

#endif

