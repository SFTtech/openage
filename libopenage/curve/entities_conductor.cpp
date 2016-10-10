// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "entities_conductor.h"

#include <algorithm>

#include "../error/error.h"

namespace openage {
namespace curve {

EntitiesConductor::EntitiesConductor(std::function<void(int, int, coord::phys2)> emerge, std::function<void(int)> vanish)
	:
	emerge{emerge},
	vanish{vanish} {
}

void EntitiesConductor::jump(GameClock::moment from, GameClock::moment to) {
	// TODO: call appear/vanish for every SPAWN/DIE (depending on the time direction)
	// TODO: for each property find closest curve to 'to' that changes it, then apply the last value or a value that is sampled from curve at the 'to' moment
}

Prediction EntitiesConductor::predict_migration(GameClock::moment current_time, GameClock::moment other_time) const {
	const auto prior = std::min(current_time, other_time);
	const auto forth = std::max(current_time, other_time);

	const auto cmp_start_cp = [](const OccurenceCurve &c, GameClock::moment p) { return c.origin.time < p; };
	const auto cmp_start_pc = [](GameClock::moment p, const OccurenceCurve &c) { return p < c.origin.time; };

	const auto cmp_end_ip = [this](size_t index, GameClock::moment p) { return end_time(this->history[index]) < p; };
	const auto cmp_end_pi = [this](GameClock::moment p, size_t index) { return p < end_time(this->history[index]); };

	ENSURE(this->history.size() == this->history_sorted_by_ends.size(), "index array out of sync");

	const auto occurences_that_start_inside_begin_it = std::lower_bound(std::begin(this->history), std::end(this->history), prior, cmp_start_cp);
	const auto occurences_that_start_inside_end_it = std::upper_bound(std::begin(this->history), std::end(this->history), forth, cmp_start_pc);

	const auto occurencesthat_finish_inside_begin_it = std::lower_bound(std::begin(this->history_sorted_by_ends), std::end(this->history_sorted_by_ends), prior, cmp_end_ip);
	const auto occurencesThat_finish_inside_end_it = std::upper_bound(std::begin(this->history_sorted_by_ends), std::end(this->history_sorted_by_ends), forth, cmp_end_pi);

	decltype(this->history_sorted_by_ends) occurences_that_only_finish_inside;
	const size_t completely_inside_begin_index = std::distance(std::begin(this->history), occurences_that_start_inside_begin_it);

	std::copy_if(occurencesthat_finish_inside_begin_it, occurencesThat_finish_inside_end_it, std::back_inserter(occurences_that_only_finish_inside), [completely_inside_begin_index](size_t index) { return index < completely_inside_begin_index; });
	std::sort(std::begin(occurences_that_only_finish_inside), std::end(occurences_that_only_finish_inside));

	const auto occurences_that_start_inside_count = std::distance(occurences_that_start_inside_begin_it, occurences_that_start_inside_end_it);
	std::vector<OccurenceCurve> trimmed(occurences_that_start_inside_count + occurences_that_only_finish_inside.size());

	using namespace std::placeholders;
	std::transform(occurences_that_start_inside_begin_it, occurences_that_start_inside_end_it, std::begin(trimmed), std::bind(&trim, _1, prior, forth));
	std::transform(std::begin(occurences_that_only_finish_inside), std::end(occurences_that_only_finish_inside), std::begin(trimmed) + occurences_that_start_inside_count, [this, prior, forth](size_t index) {
		return trim(this->history[index], prior, forth);
	});

	return trimmed;
}

void EntitiesConductor::converge(GameClock::moment current_time) {
	// TODO: perform silently all rollbacks that unapplied_history causes (simplest: jump() back, fix history, jump() forward)
	return;
}

void EntitiesConductor::append_history(const Prediction &unapplied_history) {
	this->unapplied_history.insert(std::end(this->unapplied_history), std::begin(unapplied_history), std::end(unapplied_history));
}

}} // namespace openage::curve
