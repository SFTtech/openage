// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOG_LOG_H_
#define OPENAGE_LOG_LOG_H_

#include <vector>
#include <tuple>
#include <atomic>

#include "logsource.h"
#include "logsink.h"
#include "stdout_logsink.h"

namespace openage {
namespace log {


/**
 * A globally-available "general" log source, for use by anything that isn't
 * associated with a particular logger object.
 * Initialized during init().
 */
extern LogSource *general_log_source;


/**
 * The default standard-output log sink.
 * Initialized during init().
 */
extern StdOutSink *stdout_log_sink;


/**
 * Convenience method that makes use of the 'general' LogSource.
 *
 * Invokes general->log(msg).
 */
void log(MessageBuilder &msg);


/**
 * Initializes the logger for usage.
 *
 * Using any logging functionalities before calling this method
 * results in undefined behavior (usually: messages get ignored).
 */
void init();


/**
 * Deletes stdout_log_sink.
 */
void deinit();


}} // openage::log

#endif
