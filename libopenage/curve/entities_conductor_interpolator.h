// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>

#include "game_clock.h"
#include "occurence_curve.h"

namespace openage {
namespace curve {

class EntitiesConductor;

/**
 * Obtains segments of history and interpolates them.
 *
 * Holds the current game time.
 */
class EntitiesConductorInterpolator
{
public:
	/**
	 * @param conductor holds the history of the world and can jump between moments in time
	 * @param min_frame_duration minimal length of a time segment to hold
	 */
	explicit EntitiesConductorInterpolator(EntitiesConductor &conductor, GameClock::duration min_frame_duration);

	/**
	 * Move the world to the specific moment by performing everything between current time and that moment.
	 *
	 * Does converge when loading different segment.
	 *
	 * @param m target moment
	 */
	void migrate(GameClock::moment m);

	/**
	 * Immediately mutate the world to the state of specified moment.
	 *
	 * Does converge before jumping.
	 *
	 * @param m target moment
	 */
	void jump(GameClock::moment m);

private:
	/**
	 * @param m target moment (must be outside of the current time segment)
	 */
	void expand_time_segment(GameClock::moment m);

	void play(GameClock::moment m);

	EntitiesConductor &conductor;

	GameClock::duration min_frame_duration;

	/**
	 * 'start' is included into the segment, but the 'end' is not.
	 * 'Start' <= 'end'.
	 */
	GameClock::moment current_logic_frame_start;
	GameClock::moment current_logic_frame_end;

	GameClock::moment current_time;

	Prediction predictions_since_current_logic_frame_start;
};

}} // namespace openage::curve
