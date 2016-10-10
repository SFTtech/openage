// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "entities_conductor_interpolator.h"

#include <cmath>
#include <algorithm>
#include <tuple>

#include "../error/error.h"
#include "../testing/testing.h"

#include "entities_conductor.h"

namespace openage {
namespace curve {

namespace {
GameClock::moment calc_new_frame_border(GameClock::moment desired_moment, GameClock::moment behind, GameClock::moment infront, GameClock::duration min_frame_duration) {
	ENSURE(std::max(behind, infront) - std::min(behind, infront) >= min_frame_duration, "behind..infront must be at least min_frame_duration");
	ENSURE((desired_moment < infront && infront < behind) || (behind < infront && infront <= desired_moment), "the behind..infront segment must be oriented towards the desired_moment");

	const auto frame_count = decltype(min_frame_duration)(desired_moment - infront + decltype(min_frame_duration)(infront > behind ? 1 : 0)) / (1. * min_frame_duration);
	const auto int_frame_count = infront > behind ? std::ceil(frame_count) : std::floor(frame_count);

	const auto direction_unit = infront > behind ? 1 : -1;
	const auto delta = (decltype(min_frame_duration)::rep(int_frame_count + direction_unit)) * min_frame_duration;
	static_assert(std::is_signed<decltype(delta)::rep>::value, "time representation must be a signed number");

	const auto new_infront = behind + delta > std::decay<decltype(behind)>::type{} ? behind + delta : std::decay<decltype(behind)>::type{};

	return new_infront;
}
} // namespace

EntitiesConductorInterpolator::EntitiesConductorInterpolator(EntitiesConductor &conductor, GameClock::duration min_frame_duration)
	:
	conductor{conductor},
	min_frame_duration{min_frame_duration},
	current_logic_frame_start{},
	current_logic_frame_end{},
	current_time{} {
}

void EntitiesConductorInterpolator::migrate(GameClock::moment m) {
	ENSURE(this->current_logic_frame_start <= this->current_logic_frame_end, "time frame bounds are messed up");

	if (m >= this->current_logic_frame_end || m < this->current_logic_frame_start)
		this->expand_time_segment(m);

	this->play(m);
}

void EntitiesConductorInterpolator::jump(GameClock::moment m) {
	ENSURE(this->current_logic_frame_start <= this->current_logic_frame_end, "time frame bounds are messed up");

	this->conductor.converge(this->current_time);

	this->conductor.jump(this->current_time, m);
	this->predictions_since_current_logic_frame_start.clear();
	this->current_logic_frame_start = this->current_logic_frame_end = m;
}

void EntitiesConductorInterpolator::expand_time_segment(GameClock::moment m) {
	ENSURE(this->current_logic_frame_start <= this->current_logic_frame_end, "time frame bounds are messed up");

	GameClock::moment *behind = nullptr;
	GameClock::moment *infront = nullptr;

	if (m >= this->current_logic_frame_end)
		std::tie(behind, infront) = std::make_tuple(&this->current_logic_frame_start, &this->current_logic_frame_end);
	else if (m < this->current_logic_frame_start)
		std::tie(behind, infront) = std::make_tuple(&this->current_logic_frame_end, &this->current_logic_frame_start);
	else
		ENSURE(false, "the moment is not outside of the current frame, so no frame moving needed");

	std::tie(*behind, *infront) = std::make_pair(this->current_time, calc_new_frame_border(m, *behind, *infront, this->min_frame_duration));

	this->conductor.converge(*behind);
	this->predictions_since_current_logic_frame_start = this->conductor.predict_migration(*behind, *infront);
}

void EntitiesConductorInterpolator::play(GameClock::moment m) {
}

namespace tests {
void advance_frame() {
	calc_new_frame_border(GameClock::moment{5}, GameClock::moment{0}, GameClock::moment{5}, GameClock::duration{5}) == GameClock::moment{10} || TESTFAIL;
	calc_new_frame_border(GameClock::moment{10}, GameClock::moment{0}, GameClock::moment{5}, GameClock::duration{5}) == GameClock::moment{15} || TESTFAIL;
	calc_new_frame_border(GameClock::moment{35}, GameClock::moment{10}, GameClock::moment{30}, GameClock::duration{20}) == GameClock::moment{50} || TESTFAIL;
	calc_new_frame_border(GameClock::moment{58}, GameClock::moment{10}, GameClock::moment{30}, GameClock::duration{20}) == GameClock::moment{70} || TESTFAIL;
	calc_new_frame_border(GameClock::moment{70}, GameClock::moment{10}, GameClock::moment{30}, GameClock::duration{20}) == GameClock::moment{90} || TESTFAIL;

	calc_new_frame_border(GameClock::moment{29}, GameClock::moment{40}, GameClock::moment{30}, GameClock::duration{10}) == GameClock::moment{20} || TESTFAIL;
	calc_new_frame_border(GameClock::moment{20}, GameClock::moment{40}, GameClock::moment{30}, GameClock::duration{10}) == GameClock::moment{20} || TESTFAIL;
	calc_new_frame_border(GameClock::moment{5}, GameClock::moment{40}, GameClock::moment{30}, GameClock::duration{10}) == GameClock::moment{0} || TESTFAIL;
}

} // tests

}} // namespace openage::curve
