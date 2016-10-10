// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <chrono>
#include <type_traits>

namespace openage {
namespace curve {

struct GameClock {
	using moment = std::chrono::milliseconds;
	static_assert(std::is_trivially_default_constructible<moment>::value, "instance should be trivially-default-constructible since we use it everywhere");

	using duration = std::chrono::milliseconds;
	using rep = duration::rep;
	using period = duration::period;
	using time_point = std::chrono::time_point<GameClock>;
	static const bool is_steady = false;
};

}} // namespace openage::curve
