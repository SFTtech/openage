// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>

#include "log.h"
#include "logsource.h"
#include "logsink.h"

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

	void output_log_message(const message &msg, LogSource * /*source*/) {
		this->os << msg << std::endl;
	}
};


void demo() {
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

}}} // openage::log::tests
