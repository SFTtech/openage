// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#include <cmath>

#include "player.h"
#include "score.h"
#include "team.h"
#include "../../log/log.h"

namespace openage {

Score::Score()
	:
	score{0},
	score_total{0},
	score_exploration{0},
	score_resources{0} {
}

void Score::add_score(const score_category cat, double value) {
	this->add_score(cat, static_cast<int>(std::lround(value)));
}

void Score::add_score(const score_category cat, int value) {
	this->score[static_cast<int>(cat)] += value;
	this->update_score();
}

void Score::remove_score(const score_category cat, double value) {
	this->remove_score(cat, static_cast<int>(std::lround(value)));
}

void Score::remove_score(const score_category cat, int value) {
	this->score[static_cast<int>(cat)] -= value;
	this->update_score();
}

void Score::update_map_explored(double progress) {
	this->remove_score(score_category::technology, this->score_exploration);
	this->score_exploration = progress * 1000;
	this->add_score(score_category::technology, this->score_exploration);
}

void Score::update_resources(const ResourceBundle & resources) {
	this->remove_score(score_category::economy, this->score_resources);
	this->score_resources = resources.sum() * 0.1;
	this->add_score(score_category::economy, this->score_resources);
}

void Score::update_score() {
	this->score_total = 0;
	for (int i = 0; i < static_cast<int>(score_category::SCORE_CATEGORY_COUNT); i++) {
		this->score_total += this->get_score(i);
	}
}

PlayerScore::PlayerScore(Player *player)
	:
	Score(),
	player{player} {
}

void PlayerScore::update_score() {
	Score::update_score();
	// update team score
	if (this->player->team) {
		this->player->team->score.update_score();
	}
}

TeamScore::TeamScore(Team *team)
	:
	Score(),
	team{team} {
}

void TeamScore::update_score() {
	// scores are the corresponding sums of players score
	for (int i = 0; i < static_cast<int>(score_category::SCORE_CATEGORY_COUNT); i++) {
		this->score[i] = 0;
	}
	for (auto player : this->team->get_players()) {
		for (int i = 0; i < static_cast<int>(score_category::SCORE_CATEGORY_COUNT); i++) {
			this->score[i] += player->score.get_score(i);
		}
	}
	Score::update_score();
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
