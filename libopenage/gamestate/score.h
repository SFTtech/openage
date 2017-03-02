// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>

#include "resource.h"


namespace openage {

/**
 * TODO asdf
 */
enum class score_category {
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
 * TODO asdf
 */
class Score {
public:

	Score();

	void add_score(const score_category cat, double value);

	void remove_score(const score_category cat, double value);

	// Getters

	double getScore(const score_category cat) const { return score[(int) cat]; }
	double getScore(const int index) const { return score[index]; }

	double getScoreTotal() const { return score_total; }

private:

	void update_score();

	double score[(int) score_category::SCORE_CATEGORY_COUNT];

	// generated values

	double score_total;
};

} // namespace openage

namespace std {

std::string to_string(const openage::score_category &cat);

/**
 * hasher for score_category
 */
template<> struct hash<openage::score_category> {
	typedef underlying_type<openage::score_category>::type underlying_type;
	typedef hash<underlying_type>::result_type result_type;
	result_type operator()( const openage::score_category& arg ) const {
		hash<underlying_type> hasher;
		return hasher(static_cast<underlying_type>(arg));
	}
};

} // namespace std
