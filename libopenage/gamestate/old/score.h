// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>

#include "resource.h"


namespace openage {

class Player;
class Team;

/**
 * The categories of sub-scores that sum to a player's score.
 */
enum class score_category : int {
	/** 20% of units killed cost */
	military,
	/** 20% of alive units cost and 10% of resources */
	economy,
	/** 20% of researched technologies and 10 for each 1% of map explored*/
	technology,
	/** 20% of Castles and Wonders cost */
	society,
	SCORE_CATEGORY_COUNT
};

/**
 * Keeps track of a score and all the sub-scores
 */
class Score {
public:

	Score();

	void add_score(const score_category cat, double value);
	void add_score(const score_category cat, int value);

	void remove_score(const score_category cat, double value);
	void remove_score(const score_category cat, int value);

	/**
	 * Updates map exploration precentance based sub-scores
	 */
	void update_map_explored(double progress);

	/**
	 * Updates resource based sub-scores
	 */
	void update_resources(const ResourceBundle & resources);

	/**
	 * Calculates the total score from the sub-scores.
	 * TODO update gui here
	 */
	virtual void update_score();

	// Getters

	int get_score(const score_category cat) const { return score[static_cast<int>(cat)]; }
	int get_score(const int index) const { return score[index]; }

	int get_score_total() const { return score_total; }

protected:

	int score[static_cast<int>(score_category::SCORE_CATEGORY_COUNT)];

	// generated values

	int score_total;

private:

	/** Used by update_map_explored. */
	int score_exploration;

	/** Used by update_resources. */
	int score_resources;
};


/**
 * The score of a player
 */
class PlayerScore : public Score {
public:

	PlayerScore(Player *player);

	virtual void update_score() override;

protected:

private:

	Player *player;
};


/**
 * The score of a team
 */
class TeamScore : public Score {
public:

	TeamScore(Team *team);

	virtual void update_score() override;

protected:

private:

	Team *team;
};

} // namespace openage

namespace std {

std::string to_string(const openage::score_category &cat);

/**
 * hasher for score_category
 * TODO decide if needed, not used at the moment
 */
template<>
struct hash<openage::score_category> {
	typedef underlying_type<openage::score_category>::type underlying_type;

	size_t operator()(const openage::score_category &arg) const {
		hash<underlying_type> hasher;
		return hasher(static_cast<underlying_type>(arg));
	}
};

} // namespace std
