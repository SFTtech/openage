// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <vector>

#include "../coord/phys2.h"

#include "game_clock.h"
#include "occurence_curve.h"

namespace openage {
namespace curve {
/**
 * Stores history, gives segments of it, can immediately mutate the world to the given moment.
 */
class EntitiesConductor
{
public:
	/**
	 * @param emerge function to add a unit
	 * @param vanish function to remove a unit
	 */
	EntitiesConductor(std::function<void(int, int, coord::phys2)> emerge, std::function<void(int)> vanish);

	/**
	 * Immediately mutate the world to the state of specified moment.
	 *
	 * @param current_time current time of the world
	 * @param to desired time
	 */
	void jump(GameClock::moment current_time, GameClock::moment to);

	/**
	 * Output what happens between two moments.
	 *
	 * @param current_time current time of the world
	 * @param other_time other point in time
	 * @return sorted timed occurrences from the past to the future
	 */
	Prediction predict_migration(GameClock::moment current_time, GameClock::moment other_time) const;

	/**
	 * Update the history and mutate the world to eliminate the divergence.
	 *
	 * @param current_time current time of the world
	 */
	void converge(GameClock::moment current_time);

	/**
	 * Queue up some new history.
	 *
	 * It needs to be converged to take effect.
	 *
	 * @param list of curves
	 */
	void append_history(const Prediction &unapplied_history);

private:
	std::function<void(int, int, coord::phys2)> emerge;
	std::function<void(int)> vanish;

	Prediction history;
	std::vector<size_t> history_sorted_by_ends;
	Prediction unapplied_history;
};

}} // namespace openage::curve
