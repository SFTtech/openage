// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "log.h"
#include "file_logsink.h"
#include "level.h"

#include <iostream>
#include <iomanip>
#include <thread>

#include "../util/strings.h"

namespace openage {
namespace log {
namespace tests {


class TestLogSource : public LogSource {
public:
	std::string logsource_name() {
		return "TestLogSource";
	}
};


class TestLogSink : public LogSink {
public:
	TestLogSink(std::ostream &os)
		:
		os{os} {}
private:
	std::ostream &os;

	void output_log_message(const Message &msg, LogSource * /*source*/) {
		this->os << msg << std::endl;
	}
};

/**
 * Tests log Messages through the standard output.
 */
void std_out_log_sink () {
	TestLogSource logger;
	TestLogSink sink{std::cout};

	logger.log(MSG(err) << 1337 << "asdf...");
	logger.log(MSG(crit) << util::sformat("asdf %s %d", "test", 1337));

	std::thread t0([&](){
		logger.log(MSG(info) << "this msg comes from a thread");
	});

	std::thread t1([&](){
		logger.log(MSG(dbg) << "this one, too!");
	});

	auto m = MSG(warn);
	for (int i = 0; i < 4; i++) {
		m << i << ", ";
	}

	logger.log(m);

	t0.join();
	t1.join();
}

/**
 * Creates a FileSink to also write log Messages
 * into /tmp/openage-log-test.
 * Only Messages with a level equals or higher
 * than level::err will be printed in that file.
 */
void file_log_sink () {
	TestLogSource logger;
	FileSink filelog("/tmp/openage-log-test", true);

	filelog.loglevel = log::level::err;

	// filelog should ignore these Messages
	logger.log(MSG(dbg) << "This message should not appear in openage-log-test");
	logger.log(MSG(info) << "This message should not appear in openage-log-test");
	logger.log(MSG(warn) << "This message should not appear in openage-log-test");

	// filelog should write these Messages
	logger.log(MSG(err) << "This message should appear in openage-log-test");
	logger.log(MSG(crit) << "This message should appear in openage-log-test");
}

void test() {
	std_out_log_sink();
	file_log_sink();
}

}}} // openage::log::tests
