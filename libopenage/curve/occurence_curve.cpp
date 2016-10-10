// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "occurence_curve.h"

#include <array>
#include <algorithm>
#include <string>
#include <type_traits>

#include "../error/error.h"

namespace openage {
namespace curve {

namespace
{
template <typename T, typename ...ArgTypes>
bool has(T u, ArgTypes ...args) {
	const std::array<T, sizeof...(args)> v{args...};
	return std::find(std::begin(v), std::end(v), u) != std::end(v);
}

template<Shape>
bool is(const OccurenceCurve& c);

template<>
bool is<Shape::POINT>(const OccurenceCurve& c) {
	return has(
		c.meta.kind,
		Occurence::kind::ATTACK,
		Occurence::kind::BUILD,
		Occurence::kind::DIE,
		Occurence::kind::GATHER,
		Occurence::kind::GARRISON,
		Occurence::kind::PRODUCE,
		Occurence::kind::SPAWN
	);
}

template<>
bool is<Shape::SEGMENT>(const OccurenceCurve& c) {
	return has(
		c.meta.kind,
		Occurence::kind::HEAL,
		Occurence::kind::MOVE
	);
}

void assert_failure(const OccurenceCurve& c, const char *message) {
	const auto kindVal = "'" + std::to_string(static_cast<std::underlying_type_t<decltype(c.meta.kind)>>(c.meta.kind)) + "'";
	ENSURE(false, message + kindVal);
}

template<typename T>
Values<Shape::SEGMENT, T> trim(const Values<Shape::SEGMENT, T>& v, GameClock::moment begin_time, GameClock::moment end_time, GameClock::moment prior, GameClock::moment forth) {
	const T new_initial = begin_time < prior ? v.initial + (v.final - v.initial) / ((end_time - begin_time) / (prior - begin_time)) : v.initial;
	const T new_final = end_time < forth ? v.initial + (v.final - v.initial) / ((end_time - begin_time) / (forth - begin_time)) : v.final;
	return {new_initial, new_final};
}

} // namespace

GameClock::moment end_time(const OccurenceCurve& c) {
	if (is<Shape::POINT>(c)) {
		return c.origin.time;
	} else if (is<Shape::SEGMENT>(c)) {
		return c.desc.segment.time_instances.end_time;
	} else {
		assert_failure(c, "can not deduce the end time of the curve because of its unknown kind: ");
		return c.origin.time;
	}
}

OccurenceCurve trim(const OccurenceCurve& c, GameClock::moment prior, GameClock::moment forth) {
	ENSURE(prior < forth, "non-normalized time segment");

	const auto end = end_time(c);
	ENSURE(prior <= c.origin.time && end < forth, "curve does not intersect the time segment");

	auto result = c;

	if (is<Shape::POINT>(c)) {
		/**
		 * The curve intersects the time segment and curve is a point, therefore it's inside it.
		 */
	} else if (is<Shape::SEGMENT>(c)) {
		result.origin.time = std::min(c.origin.time, prior);
		result.desc.segment.time_instances.end_time = std::max(c.desc.segment.time_instances.end_time, forth);

		switch (c.meta.kind) {
		case Occurence::kind::HEAL:
			result.desc.segment.value.amount = trim(c.desc.segment.value.amount, c.origin.time, end, prior, forth);
			break;

		case Occurence::kind::MOVE:
			result.desc.segment.value.pos = trim(c.desc.segment.value.pos, c.origin.time, end, prior, forth);
			break;

		default:
			assert_failure(c, "can not trim segment-curve because of its wrong kind: ");
			break;
		}
	} else {
		assert_failure(c, "can not trim curve because of its unknown kind: ");
	}

	return result;
}

}} // namespace openage::curve
