// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include <chrono>
#include <thread>

namespace openage {
namespace test {

void benchmark() {
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1ms);
}

}} // openage::test
