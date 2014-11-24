// Copyright 2013-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOG_H_
#define OPENAGE_LOG_H_

#include "util/queue.h"
#include "util/demangle.h"
#include "util/strings.h"

#include <iostream>
#include <fstream>
#include <utility>
#include <ctime>
#include <string>
#include <cstdio>
#include <cstddef>
#include <cstdarg>
#include <type_traits>
#include <typeinfo>
#include <thread>

namespace openage {
namespace log {

enum class LogLevel {
	 FATAL =0
	,ERROR =1
	,WARN  =2
	,IMP   =3
	,MSG   =4
	,DBG2  =5
	,DBG1  =6
	,DBG0  =7
	,DBG   =7
};

struct LogLevelInfo {
	const char *name;
	const char *col;
};

constexpr const LogLevelInfo loglevels[] {
	 {"FATAL", "31;1"}
	,{"ERROR", "31"}
	,{"WARN ", "34;1"}
	,{"IMP  ", "36;1"}
	,{"MSG  ", "36"}
	,{"DBG2 ", "32"}
	,{"DBG1 ", "32"}
	,{"DBG0 ", "32"}
};

using timepoint_type = std::chrono::time_point<std::chrono::system_clock>;

/**
 * Log entry type: containing various informations
 */
struct logentry_type {
	LogLevel 	level;
	const char*	logger_tag;
	std::thread::id thread_id;
	const char*	filename;
	const char*	function_name;
	size_t		line_number;
	timepoint_type	timepoint;
	std::string	message;
};

/**
 * Helper function to make a log entry and to format its message
 */
template <typename... Args>
logentry_type make_logentry(
		  LogLevel              level
		, const char*           logger_tag
		, std::thread::id       thread_id
		, const char*           filename
		, const char*           function_name
		, size_t                line_number
		, timepoint_type const& timepoint
		, const char*           fmt
		, Args&&...             args
) {
	// additionnal nullptr parameter to disable warning: Format string is not a string literal (potentially insecure)
	// in case of empty args...
	// The format has already been checked by the format_checker function (see macro definitions below)
	return logentry_type{level, logger_tag, thread_id, filename, function_name, line_number, timepoint
                              , util::string_format(fmt, std::forward<Args>(args)..., nullptr)};
}

/**
 * Queue type used by AsyncLogger
 */
using queue_type = openage::util::Queue<logentry_type>;


/**
 * Type list used to configure this policy-based logger system
 */
template <typename... Loggers>
struct type_list {};


/**
 * Meta-function returns true if the first parameter 'Type' is in the type list given in the following parameters
 */
template <typename Type, typename...>
struct is_in_typelist {
	static const constexpr bool value = false;
};

template <typename Type, typename Head, typename... Tail>
struct is_in_typelist<Type, Head, Tail...> {
	static const constexpr bool value = std::is_same<Type, Head>::value || is_in_typelist<Type, Tail...>::value;
};

template <typename Type, typename...Args>
struct is_in_typelist<Type, type_list<Args...>> {
	static const constexpr bool value = is_in_typelist<Type, Args...>::value;
};


/**
 * Meta-function returns the size of the type list in parameter
 */
template <typename...>
struct sizeof_type_list {
	static const constexpr size_t value = 0;
};

template <typename... Args>
struct sizeof_type_list<type_list<Args...>> {
	static const constexpr size_t value = sizeof...(Args);
};

/**
 * Trait class used to configure this policy-based logger system
 * This traits defines some default types/values that may have to be specialized by the user
 */
template <typename T>
struct configure {
	using logger_list  = void;
#ifdef NDEBUG
	static const constexpr LogLevel log_level = LogLevel::MSG;
#else
	static const constexpr LogLevel log_level = LogLevel::DBG;
#endif
};

/**
 * The LogManager basically holds a type list of loggers and forward its static log function to the loggers static log function.
 * The type list of the logger is provided by the 'configure' trait class that has to be specialized for LogManager by the user.
 * configure<LogManager>::logger_list is void by default to force the user to specialize the trait class and provide a valid logger list.
 */
class LogManager {
public:
	template <typename ThisType, typename LogTag>
	static void log(logentry_type const & entry) {
		log<LogTag>(entry, typename configure<ThisType>::logger_list{});
	}

private:
	template <typename LogTag, typename... Loggers>
	static void log(logentry_type const & entry, type_list<Loggers...>) {
		log_imp<Loggers...>:: template log<LogTag>(entry);
	}

	template <typename...>
	struct log_imp {
		template <typename LogTag>
		static void log(logentry_type const &) {}
	};

	template <typename HeadLogger, typename... Tail>
	struct log_imp<HeadLogger, Tail...> {
		template <typename LogTag>
		static void log(logentry_type const & entry) {
			HeadLogger:: template log<LogTag>(entry);
			log_imp<Tail...>:: template log<LogTag>(entry);
		}
	};
};

// Minimum macro-glue required to manage variadic macros properly :o)
// Note: Currently these macros handle a maximum of 40 (fourty!) parameters.
/**
 * \def OPENAGE_VARARG_HEAD(...)
 * Return the first variadic argument if present or nothing
 */
#define OPENAGE_VARARG_HEAD( ...) OPENAGE_VARARG_HEAD_HELPER(__VA_ARGS__, throwaway)

/**
 * \def OPENAGE_VARARG_TAIL(...) 
 * Return the tail of the variadic arguments (all arguments except the first one)
 * The argument list returned is preceded by a ',' if non empty
 */
#define OPENAGE_VARARG_TAIL(...) OPENAGE_VARARG_TAIL_HELPER(OPENAGE_VARARG_TAIL_NUM(__VA_ARGS__), __VA_ARGS__)

/**
 * \def OPENAGE_VARARG_TAIL2(...)
 * Return the tail of the variadic arguments (all arguments except the first one) or nothing
 * if no more than one argument present.
 */
#define OPENAGE_VARARG_TAIL2(...) OPENAGE_VARARG_TAIL_HELPER(OPENAGE_VARARG_TAIL_NUM2(__VA_ARGS__), __VA_ARGS__)

// Macro-glue helpers: use the "pigeonhole" principle
#define OPENAGE_VARARG_HEAD_HELPER(HEAD, ...)           HEAD
#define OPENAGE_VARARG_TAIL_HELPER(SELECTER, ...)       OPENAGE_VARARG_TAIL_HELPER2(SELECTER, __VA_ARGS__)
#define OPENAGE_VARARG_TAIL_HELPER2(SELECTER, ...)      OPENAGE_VARARG_TAIL_HELPER_##SELECTER(__VA_ARGS__)
#define OPENAGE_VARARG_TAIL_HELPER_ONE(first)
#define OPENAGE_VARARG_TAIL_HELPER_SEVERAL(first, ...)  , __VA_ARGS__
#define OPENAGE_VARARG_TAIL_HELPER_SEVERAL2(first, ...) __VA_ARGS__

// The following macro is used to select the correct macro 
// between OPENAGE_VARARG_TAIL_HELPER_ONE and OPENAGE_VARARG_TAIL_HELPER_SEVERAL
#define OPENAGE_VARARG_TAIL_NUM(...) \
	    OPENAGE_VARARG_TAIL_SELECT_40TH(__VA_ARGS__, \
                                            SEVERAL, SEVERAL, SEVERAL, SEVERAL, SEVERAL, \
                                            SEVERAL, SEVERAL, SEVERAL, SEVERAL, SEVERAL, \
                                            SEVERAL, SEVERAL, SEVERAL, SEVERAL, SEVERAL, \
                                            SEVERAL, SEVERAL, SEVERAL, SEVERAL, SEVERAL, \
                                            SEVERAL, SEVERAL, SEVERAL, SEVERAL, SEVERAL, \
                                            SEVERAL, SEVERAL, SEVERAL, SEVERAL, SEVERAL, \
                                            SEVERAL, SEVERAL, SEVERAL, SEVERAL, SEVERAL, \
                                            SEVERAL, SEVERAL, SEVERAL, ONE, throwaway)

// The following macro is used to select the correct macro 
// between OPENAGE_VARARG_TAIL_HELPER_ONE and OPENAGE_VARARG_TAIL_HELPER_SEVERAL2
#define OPENAGE_VARARG_TAIL_NUM2(...) \
	    OPENAGE_VARARG_TAIL_SELECT_40TH(__VA_ARGS__, \
                                            SEVERAL2, SEVERAL2, SEVERAL2, SEVERAL2, SEVERAL2, \
                                            SEVERAL2, SEVERAL2, SEVERAL2, SEVERAL2, SEVERAL2, \
                                            SEVERAL2, SEVERAL2, SEVERAL2, SEVERAL2, SEVERAL2, \
                                            SEVERAL2, SEVERAL2, SEVERAL2, SEVERAL2, SEVERAL2, \
                                            SEVERAL2, SEVERAL2, SEVERAL2, SEVERAL2, SEVERAL2, \
                                            SEVERAL2, SEVERAL2, SEVERAL2, SEVERAL2, SEVERAL2, \
                                            SEVERAL2, SEVERAL2, SEVERAL2, SEVERAL2, SEVERAL2, \
                                            SEVERAL2, SEVERAL2, SEVERAL2, ONE, throwaway)

// Return the 40th argument
#define OPENAGE_VARARG_TAIL_SELECT_40TH(a1,  a2,  a3,  a4,  a5,  a6,  a7,  a8,  a9,  a10, \
                                        a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, \
                                        a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, \
                                        a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, ...) a40

// Define some middle-end log macros
#define OPENAGE_STRINGIZE(STR)           #STR 
#define OPENAGE_FILELINE_A(LINE)         __FILE__ "(" OPENAGE_STRINGIZE(LINE) "): "
#define OPENAGE_FILELINE(LINE)           OPENAGE_FILELINE_A(LINE)
#define OPENAGE_LOG_PREFIX(LINE)         OPENAGE_FILELINE(LINE)
#define LOG_TEMPLATE_PARAMS(...)         ::openage::log::LogLevel::OPENAGE_VARARG_HEAD(__VA_ARGS__) OPENAGE_VARARG_TAIL(__VA_ARGS__)
#define LOG_FORMAT(...)				     OPENAGE_VARARG_HEAD(OPENAGE_VARARG_TAIL2(__VA_ARGS__)) "\n"
#define LOG_PREFIXED_FORMAT(...)		 "%s: %s" LOG_FORMAT(__VA_ARGS__)
#define LOG_PARAMS(LEVEL, ...)           OPENAGE_VARARG_TAIL(OPENAGE_VARARG_TAIL2(__VA_ARGS__))

#define LOG_SHORT_TAG(LEVEL, ...) log<LOG_TEMPLATE_PARAMS(LEVEL, OPENAGE_VARARG_HEAD(__VA_ARGS__))>::ger( \
		__FILE__, __func__, __LINE__, LOG_PREFIXED_FORMAT(__VA_ARGS__), LOG_FORMAT(__VA_ARGS__), OPENAGE_LOG_PREFIX(__LINE__) LOG_PARAMS(LEVEL, __VA_ARGS__));\
		::openage::log::format_checker(LOG_FORMAT(__VA_ARGS__) LOG_PARAMS(LEVEL, __VA_ARGS__))
#define LOG_SHORT(LEVEL, ...) LOG_SHORT_TAG(LEVEL, ::openage::log::root, __VA_ARGS__)

// Define front-end log macros
// These macros logs to 'openage::log::root' logger tag
#define dbg(...) 	LOG_SHORT(DBG, __VA_ARGS__)
#define dbg0(...) 	LOG_SHORT(DBG0, __VA_ARGS__)
#define dbg1(...) 	LOG_SHORT(DBG1, __VA_ARGS__)
#define dbg2(...) 	LOG_SHORT(DBG2, __VA_ARGS__)
#define msg(...)	LOG_SHORT(MSG, __VA_ARGS__)
#define imp(...)	LOG_SHORT(IMP, __VA_ARGS__)
#define warn(...)	LOG_SHORT(WARN, __VA_ARGS__)
#define err(...)	LOG_SHORT(ERROR, __VA_ARGS__)
#define fatal(...)	LOG_SHORT(FATAL, __VA_ARGS__)

// These macros logs to the logger tag given in first parameter
// For example: log::tmsg(YourMom, "Hello World!") will use YourMom as a logger tag
// Logger tags are associated with a LogLevel that can be used to filter messages.
// Logger tags can also be associated with a particular Logger
// All of this happens in the 'configure' trait class.
#define tag(TAG, LEVEL, ...) LOG_SHORT_TAG(LEVEL, TAG, __VA_ARGS__)
#define tdbg(TAG, ...)       LOG_SHORT_TAG(DBG, TAG, __VA_ARGS__)
#define tdbg0(TAG, ...)      LOG_SHORT_TAG(DBG0, TAG, __VA_ARGS__)
#define tdbg1(TAG, ...)      LOG_SHORT_TAG(DBG1, TAG, __VA_ARGS__)
#define tdbg2(TAG, ...)      LOG_SHORT_TAG(DBG2, TAG, __VA_ARGS__)
#define tmsg(TAG, ...)       LOG_SHORT_TAG(MSG, TAG, __VA_ARGS__)
#define timp(TAG, ...)       LOG_SHORT_TAG(IMP, TAG, __VA_ARGS__)
#define twarn(TAG, ...)      LOG_SHORT_TAG(WARN, TAG, __VA_ARGS__)
#define terr(TAG, ...)       LOG_SHORT_TAG(ERROR, TAG, __VA_ARGS__)
#define tfatal(TAG, ...)     LOG_SHORT_TAG(FATAL, TAG, __VA_ARGS__)

// Logger output policies 
/**
 * Console output policy
 */
struct cout {
	static std::ostream& stream() {
		return std::cout;
	}
};

/**
 * File output policy
 * This policy use a template parameter to define the filename.
 */
template <typename TemplateString>
// requires TemplateString::get() to return a const char*
struct fout  {
	static std::ostream& stream() {
		static auto&& f = std::ofstream(TemplateString::get());
		return f;
	}
};

/**
 * \def OPENAGE_TPL_STRING
 *  Macro helper to define a "template string parameters"
 */
#define OPENAGE_TPL_STRING(name, str) struct name { static const char* get() {return str;}}

/**
 * noop_printer policy can replace any printer policy to disable the associated output
 */
struct noop_printer {
	static std::string print(logentry_type const &) {return {};}
};

/**
 * Printer policy that returns a the time as a string
 */
struct time_printer {
	static std::string print(logentry_type const& entry) {
		return util::timepoint_to_string(entry.timepoint) + " ";
	}
};

/**
 * Printer policy that returns date and time as a string
 */
struct datetime_printer {
	static std::string print(logentry_type const& entry) {
		return util::timepoint_to_string(entry.timepoint, true) + " ";
	}
};

/**
 * Printer policy that returns the log entry logger tag as a string
 */
struct tag_printer {
	static std::string print(logentry_type const& entry) {
		return util::demangle(entry.logger_tag) + " - ";
	}
};

/**
 * Printer policy that returns the thread id of the log entry as a string
 */
struct thread_printer {
	static std::string print(logentry_type const& entry) {
		return std::string("ThreadID: ") + util::threadid_to_string(entry.thread_id) + " - ";
	}
};

/**
 * Printer policy that returns the log level as a string (without colors)
 */
struct loglevel_printer {
	static std::string print(logentry_type const& entry) {
		return std::string(loglevels[(int)entry.level].name) + " ";
	}
};

/**
 * Printer policy that returns the colored log level as a string
 */
struct colored_loglevel_printer {
	static std::string print(logentry_type const& entry) {
		return std::string("\x1b[") + loglevels[(int)entry.level].col + "m" + loglevels[(int)entry.level].name + "\x1b[m: ";
	}
};

/**
 * Printer policy that returns function name and line number informations as a string
 */
struct origin_printer {
	static std::string print(logentry_type const& entry) {
		return std::string(entry.function_name) + "(" + std::to_string(entry.line_number) + ") ";
	}
};


/**
 * AsyncLogger is a policy based Logger
 * Output: This policy is required to define 'std::ostream& stream()' static member function.
 *	   The	Output::stream() is used as an output for the logger.
 * *Printer : Theses policies are required to define '<unspecified-type> print()' static member function (whatever they print).
 *	      The print() static function may return a value of any type such that the expression "Output::stream() << *Printer::print()" is valid.
 *	      Typically, print() returns a std::string.
 * LoggersTagsIn : Type list of the logger tags this logger has to accept. All logger tags are considered by default (empty type list).
 * LoggersTagOut : Type list of the logger tags this logger has to reject.
 */
template <typename Output
	, typename DatePrinter
	, typename TagPrinter
	, typename ThreadPrinter
	, typename LogLevelPrinter
	, typename OriginPrinter
	, typename LoggersTagsIn=type_list<>
	, typename LoggersTagsOut=type_list<>
>
class AsyncLogger {
public:
	using log_queue_type = queue_type;
	using log_entry_type = typename log_queue_type::value_type;
	
	static AsyncLogger& Get() {
		static AsyncLogger unique{};
		return unique;
	}

	template <typename LoggerTag>
	static void log(logentry_type const& entry) {
       		log_imp<LoggerTag>::log(entry);
	}
	
	AsyncLogger(AsyncLogger const&) = delete;
	AsyncLogger(AsyncLogger&&)      = delete;
	AsyncLogger& operator=(AsyncLogger const&) = delete;
	AsyncLogger& operator=(AsyncLogger&&)      = delete;

	~AsyncLogger() { 
		this->queue.abort();
		// We shouln't have to notify any thread
		// Doing so anyway would trigger an helgrind error..
		// this->queue.notify();
		this->th.join();
	}
private:
	AsyncLogger() :
		 th([this]() {
			logentry_type entry;
			while(!this->queue.is_aborted()) {
				while(this->queue.pop(entry, std::chrono::milliseconds(100))) {
					Output::stream() << DatePrinter::print(entry);
					Output::stream() << TagPrinter::print(entry);
					Output::stream() << ThreadPrinter::print(entry);
					Output::stream() << LogLevelPrinter::print(entry);
					Output::stream() << OriginPrinter::print(entry);
					Output::stream() << entry.message;
				}
			}
		})
	{}
	
	// Since this->thread access this->queue in the constructor, the queue has to be constructed before the thread.
	// Attributes definition order is important here!
	log_queue_type queue;
	std::thread th;

	template <typename LoggerTag, class Enable=void>
	struct log_imp {
		static void log(logentry_type const&) {}
	};

	template <typename LoggerTag>
	struct log_imp<LoggerTag, typename std::enable_if<
						   (is_in_typelist<LoggerTag, LoggersTagsIn>::value
							|| sizeof_type_list<LoggersTagsIn>::value == 0)
						&& !is_in_typelist<LoggerTag, LoggersTagsOut>::value>::type> {
		static void log(logentry_type const & log_entry) {
			Get().queue.push(log_entry);
		}
	};
};

/**
 * The default logger tag (used by log::dbg, etc...)
 */
struct root {};

/**
 * Format (printf-like) compile time checker (uses GCC extension)
 * This is needed as variadic template functions cannot have a format attribute.
 */
__attribute__ ((format (printf, 1, 2)))
inline void format_checker(const char*, ...)
{}

/**
 * Logger system entry point. By default not enabled and do nothing
 */
template <LogLevel lvl, class LogTag=root, class Enabled=void>
struct log{
	static void ger(...) {}
};

/**
 * Logger system entry point. Enabled thanks to std::enable_if
 */
template <LogLevel lvl, class LogTag>
struct log<lvl, LogTag, typename std::enable_if<(lvl <= configure<LogTag>::log_level)>::type> {
	template <typename... Args>
	static void ger(const char* filename, const char* function_name, size_t lineno, const char* prefixed_fmt, const char* fmt, const char* log_prefix, Args&&... args)
	{
		auto now = std::chrono::system_clock::now();
#		ifndef OPENAGE_LOG_BAREBONE 
		// unused parameters
		(void)log_prefix;
		(void)prefixed_fmt;
		auto entry = make_logentry(lvl, typeid(LogTag).name(), std::this_thread::get_id(), filename, function_name, lineno, now, fmt, std::forward<Args>(args)...);
		LogManager::log<LogManager, LogTag>(entry);
#		else  // if OPENAGE_LOG_BAREBONE is defined, use a simple logger system instead
		// unused parameters
		(void)filename;
		(void)function_name;
		(void)lineno;
		(void)fmt;	
		printf("\x1b[%sm", loglevels[(int)lvl].col);
		printf("%s ", util::timepoint_to_string(now).c_str());
		printf(prefixed_fmt, loglevels[(int)lvl].name, log_prefix, std::forward<Args>(args)...);
		printf("\x1b[m");
#		endif
	}
};

} // namespace openage
} // namespace log

// Should we let the user include this file himself??
#include "log_configure.h"

#endif

