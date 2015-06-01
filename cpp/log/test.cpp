// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "log.h"
#include "file_logsink.h"

#include <thread>
#include <algorithm>
#include <cstring>
#include <unistd.h>

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
int std_out_log_sink () {
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

	return 0;
}


/**
 * Creates a FileSink to also write log Messages
 * into /tmp/openage-log-test.
 * Only Messages with a level equals or higher
 * than level::err will be printed in that file.
 */
int file_log_sink () {
	char filelog_name[20];
	char buffer[256];
	int filedes = -1, result = 0;

	memset(filelog_name, 0, sizeof(filelog_name));
	memset(buffer, 0, sizeof(buffer));

	strncpy(filelog_name, "/tmp/log-test-XXXXXX", 20);

	filedes = mkstemp(filelog_name);

	TestLogSource logger;
	FileSink filelog(filelog_name, false);
	filelog.loglevel = log::level::err;

	// filelog should ignore these Messages
	logger.log(MSG(dbg) << "This message should not appear in openage-log-test");
	logger.log(MSG(info) << "This message should not appear in openage-log-test");
	logger.log(MSG(warn) << "This message should not appear in openage-log-test");

	// filelog should write these Messages
	logger.log(MSG(err) << "This message should appear in openage-log-test");
	logger.log(MSG(crit) << "This message should appear in openage-log-test");

	std::string line;
	std::ifstream infile(filelog_name);

	while (std::getline(infile, line)) {
		if ((line.find("ERR") == std::string::npos) && (line.find("CRIT") == std::string::npos)) {
			result = -1;
			break;
		}
	}

	unlink(filelog_name);
	return result;
}


/**
 * Tests that the FileSink doesn't write any
 * message into the the filelog because it
 * has a too low log level.
 */
int empty_file_log_sink () {
	char filelog_name[20];
	char buffer[256];
	int filedes = -1, result = 0;

	memset(filelog_name, 0, sizeof(filelog_name));
	memset(buffer, 0, sizeof(buffer));

	strncpy(filelog_name, "/tmp/log-test-XXXXXX", 20);

	filedes = mkstemp(filelog_name);

	TestLogSource logger;
	FileSink filelog(filelog_name, false);
	filelog.loglevel = log::level::crit;

	// filelog should ignore all these Messages
	logger.log(MSG(dbg) << "This message should not appear in openage-log-test");
	logger.log(MSG(info) << "This message should not appear in openage-log-test");
	logger.log(MSG(warn) << "This message should not appear in openage-log-test");
	logger.log(MSG(err) << "This message should not appear in openage-log-test");

	std::ifstream infile(filelog_name); 
	int lines = std::count(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>(), '\n');

	unlink(filelog_name);

	if (lines != 0) {
		result = -1;
	}

	return result;
}


void test() {
	const char *testname;

	if (std_out_log_sink() == -1) {
		testname = "std_out_log_sink";
		goto out;
	} else if (file_log_sink() == -1) {
		testname = "file_log_sink";
		goto out;
	} else if (empty_file_log_sink() == -1) {
		testname = "empty_file_log_sink";
		goto out;
	}

	return;

out:
	log::log(MSG(err) << testname << " failed");
	throw "log tests failed";
}

}}} // openage::log::tests
