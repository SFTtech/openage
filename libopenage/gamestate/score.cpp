// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include <cmath>

#include "player.h"
#include "score.h"
#include "team.h"


namespace openage {

Score::Score()
	:
	score{0},
	score_total{0} {
}

void Score::add_score(const score_category cat, double value) {
	this->add_score(cat, (int) std::lround(value));
}

void Score::add_score(const score_category cat, int value) {
	this->score[(int) cat] += value;
}

void Score::remove_score(const score_category cat, double value) {
	this->remove_score(cat, (int) std::lround(value));
}

void Score::remove_score(const score_category cat, int value) {
	this->score[(int) cat] -= value;
}

void Score::update_score() {
	this->score_total = 0;
	for (int i=0; i<(int) score_category::SCORE_CATEGORY_COUNT; i++) {
		this->score_total += this->getScore(i);
	}
}

ScoreTeam::ScoreTeam(Team *team)
	:
	Score(),
	team{team} {
}

void ScoreTeam::update_score() {
	this->score_total = 0;
	for (int i=0; i<(int) score_category::SCORE_CATEGORY_COUNT; i++) {
		this->score[i] = 0;
	}
	// scores are the corresponding sums of players score
	for (auto player : team->get_players()) {
		for (int i=0; i<(int) score_category::SCORE_CATEGORY_COUNT; i++) {
			this->score[i] += player->score.getScore(i);
		}
		this->score_total += player->score.getScoreTotal();
	}
}

} // openage

namespace std {

string to_string(const openage::score_category &cat) {
	switch (cat) {
	case openage::score_category::military:
		return "military";
	case openage::score_category::economy:
		return "economy";
	case openage::score_category::technology:
		return "technology";
	case openage::score_category::society:
		return "society";
	default:
		return "unknown";
	}
}

} // namespace std
