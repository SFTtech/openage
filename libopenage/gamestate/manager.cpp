// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "manager.h"

#include "log/log.h"

#include "gamestate/component/internal/command_queue.h"
#include "gamestate/component/types.h"
#include "gamestate/game_entity.h"


namespace openage::gamestate {

GameEntityManager::GameEntityManager(const std::shared_ptr<openage::event::EventLoop> &loop,
                                     const std::shared_ptr<GameEntity> &game_entity) :
	event::EventEntity{loop},
	game_entity{game_entity} {}

void GameEntityManager::run_activity_system() {
	log::log(DBG << "Running activity system for entity " << this->game_entity->get_id());
}

size_t GameEntityManager::id() const {
	// TODO
	return this->game_entity->get_id();
}

std::string GameEntityManager::idstr() const {
	// TODO
	return "manager";
}

} // namespace openage::gamestate
