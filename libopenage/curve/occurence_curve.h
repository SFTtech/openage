// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <utility>
#include <type_traits>
#include <cstdint>
#include <vector>

#include "../coord/phys2.h"

#include "game_clock.h"
#include "occurence.h"

namespace openage {
namespace curve {

/**
 * TODO: establish int sizes
 */
struct OccurenceCurveMetadata {
	/**
	 * id of the concerned unit (optional)
	 */
	uint32_t unit_id;

	/**
	 * id of an additional unit (optional)
	 */
	uint32_t aux_unit_id;

	/**
	 * type like 'move', 'attack', etc.
	 */
	Occurence::kind kind;
};

enum class Shape {
	POINT,
	SEGMENT,
};

struct Origin {
	GameClock::moment time;
};

template<Shape>
struct TimeInstances;

template<>
struct TimeInstances<Shape::POINT> {
};

template<>
struct TimeInstances<Shape::SEGMENT> {
	GameClock::moment end_time;
};

template<Shape, typename T = void, typename N = std::integral_constant<bool, false>::type>
struct Values {
};

template<typename T>
struct Values<Shape::POINT, T, typename std::integral_constant<bool, std::is_same<T, void>::value>::type> {
	T val;
};


template<typename T>
struct Values<Shape::SEGMENT, T, typename std::integral_constant<bool, std::is_same<T, void>::value>::type> {
	T initial;
	T final;
};

/**
 * TODO: replace with a std::variant
 */
template<Shape S>
struct ControlPoints {
	TimeInstances<S> time_instances;
	union Value {
		Values<S> empty;
		Values<S, int> amount;
		Values<S, coord::phys2> pos;
	} value;
};

/**
 * TODO: replace with a std::variant
 * TODO: establish int sizes
 */
struct OccurenceCurve {
	OccurenceCurveMetadata meta;

	Origin origin;

	union {
		ControlPoints<Shape::POINT> point;
		ControlPoints<Shape::SEGMENT> segment;
	} desc;
};

using Prediction = std::vector<OccurenceCurve>;

GameClock::moment end_time(const OccurenceCurve& c);

/**
 * The curve must intersect the time segment.
 */
OccurenceCurve trim(const OccurenceCurve& c, GameClock::moment prior, GameClock::moment forth);

}} // namespace openage::curve
