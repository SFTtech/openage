// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "logging.h"

#include <iostream>
#include <iomanip>
#include <thread>

namespace openage {
namespace logging {
namespace test {


class TestLogger : public Logger {
public:
	std::string logger_name() {
		return "TestLogger";
	}
};


class TestLogSink : public LogSink {
public:
	TestLogSink(std::ostream &os)
		:
		os{os} {}
private:
	std::ostream &os;

	void output_log_message(const Message &msg, Logger * /*source*/) {
		this->os << msg << std::endl;
	}
};


void test() {
	TestLogger logger;
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

}}} // openage::logging::test
