// Copyright 2016-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <unordered_map>
#include <vector>


#include "score.h"


namespace openage {

class Player;

/**
 * Types of membership
 */
enum class member_type {
	leader,
	member,
	recruit,
	none // pseudo type
};

/**
 * A team of players
 */
class Team {
public:
	Team(unsigned int id);
	Team(unsigned int id, std::string name);
	Team(unsigned int id, std::string name, Player *leader);

	/**
	 * unique id of the team
	 */
	const unsigned int id;

	/**
	 * visible name of this team
	 */
	const std::string name;

	bool operator ==(const Team &other) const;


	void add_member(Player &player, const member_type type);

	void change_member_type(Player &player, const member_type type);

	bool is_member(const Player &player) const;

	void remove_member(Player &player);

	member_type get_member_type(Player &player);

	/**
	 * TODO find a better way to get all the players
	 */
	std::vector<const Player*> get_players() const;

	/**
	 * The score of the team, based on the team's players score.
	 */
	TeamScore score;

private:

	std::unordered_map<const Player*, member_type> members;

};

} // openage
