// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "score.h"


namespace openage {

Score::Score()
	:
	score{0},
	score_total{0} {
}

void Score::add_score(const score_category cat, double value) {
	this->score[(int) cat] += value;
}

void Score::remove_score(const score_category cat, double value) {
	this->score[(int) cat] -= value;
}

void Score::update_score() {
	score_total = 0;
	for (int i=0; i<(int) score_category::SCORE_CATEGORY_COUNT; i++) {
		score_total += this->getScore(i);
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
