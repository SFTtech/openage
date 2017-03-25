// Copyright 2016-2017 the openage authors. See copying.md for legal info.

#include "team.h"
#include "player.h"
#include "score.h"


namespace openage {

Team::Team(unsigned int id)
	:
	Team{id, "Anonymous Team", nullptr} {}

Team::Team(unsigned int id, std::string name)
	:
	Team{id, name, nullptr} {}

Team::Team(unsigned int id, std::string name, Player *leader)
	:
	id{id},
	name{name},
	score{this} {

	if (leader) {
		this->add_member(*leader, member_type::leader);
	}
}

bool Team::operator ==(const Team &other) const {
	return this->id == other.id;
}

void Team::add_member(Player &player, const member_type type) {
	// if already exists, replace member type
	this->members[&player] = type;
	// change player team pointer
	player.team = this;
}

void Team::change_member_type(Player &player, const member_type type) {
	auto p = this->members.find(&player);
	if (p != this->members.end()) {
		this->members[&player] = type;
	}
}

bool Team::is_member(const Player &player) const {
	auto p = this->members.find(&player);
	return (p != this->members.end());
}

void Team::remove_member(Player &player) {
	this->members.erase(&player);
	// change player team pointer
	player.team = nullptr;
}

member_type Team::get_member_type(Player &player) {
	auto p = this->members.find(&player);
	if (p != this->members.end()) {
		return this->members[&player];
	}
	// return pseudo member type for completion
	return member_type::none;
}

std::vector<const Player*> Team::get_players() const {
	std::vector<const Player*> players;
	for (auto& i : members) {
		players.push_back(i.first);
	}
	return players;
}

} // openage
