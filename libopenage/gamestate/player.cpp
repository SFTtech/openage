// Copyright 2018-2023 the openage authors. See copying.md for legal info.

#include "player.h"

#include "nyan/nyan.h"


namespace openage::gamestate {

Player::Player(player_id_t id,
               const std::shared_ptr<nyan::View> &db_view) :
	id{id},
	db_view{db_view} {
}

std::shared_ptr<Player> Player::copy(entity_id_t id) {
	auto copy = std::shared_ptr<Player>(new Player(*this));
	copy->set_id(id);

	return copy;
}

player_id_t Player::get_id() const {
	return this->id;
}

const std::shared_ptr<nyan::View> &Player::get_db_view() const {
	return this->db_view;
}

void Player::set_id(entity_id_t id) {
	this->id = id;
}

} // namespace openage::gamestate
