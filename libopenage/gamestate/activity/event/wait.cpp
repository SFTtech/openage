// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "wait.h"

#include "event/event_loop.h"
#include "event/evententity.h"
#include "gamestate/game_entity.h"
#include "gamestate/game_state.h"
#include "gamestate/manager.h"


namespace openage::gamestate::activity {

std::shared_ptr<openage::event::Event> primer_wait(const time::time_t &time,
                                                   const std::shared_ptr<GameEntity> &entity,
                                                   const std::shared_ptr<openage::event::EventLoop> &loop,
                                                   const std::shared_ptr<gamestate::GameState> &state,
                                                   size_t next_id) {
	openage::event::EventHandler::param_map::map_t params{{"next", next_id}};
	auto ev = loop->create_event("game.wait",
	                             entity->get_manager(),
	                             state,
	                             time,
	                             params);

	return ev;
};

} // namespace openage::gamestate::activity
