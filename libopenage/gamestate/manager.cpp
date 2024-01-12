// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "manager.h"

#include "log/log.h"
#include "log/message.h"

#include "gamestate/component/internal/command_queue.h"
#include "gamestate/game_entity.h"
#include "gamestate/system/activity.h"


namespace openage::gamestate {

GameEntityManager::GameEntityManager(const std::shared_ptr<openage::event::EventLoop> &loop,
                                     const std::shared_ptr<openage::gamestate::GameState> &state,
                                     const std::shared_ptr<GameEntity> &game_entity) :
	event::EventEntity{loop},
	loop{loop},
	state{state},
	game_entity{game_entity} {}

void GameEntityManager::run_activity_system(const time::time_t &time,
                                            const std::optional<openage::event::EventHandler::param_map> &ev_params) {
	log::log(DBG << "Running activity system for entity " << this->game_entity->get_id());
	system::Activity::advance(time, this->game_entity, this->loop, this->state, ev_params);
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
