// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "command_in_queue.h"

#include "event/event_loop.h"
#include "event/evententity.h"
#include "gamestate/component/internal/command_queue.h"
#include "gamestate/game_entity.h"
#include "gamestate/game_state.h"
#include "gamestate/manager.h"


namespace openage::gamestate::activity {

std::shared_ptr<openage::event::Event> primer_command_in_queue(const time::time_t &,
                                                               const std::shared_ptr<gamestate::GameEntity> &entity,
                                                               const std::shared_ptr<openage::event::EventLoop> &loop,
                                                               const std::shared_ptr<gamestate::GameState> &state,
                                                               size_t next_id) {
	openage::event::EventHandler::param_map::map_t params{{"next", next_id}}; // move->get_id();
	auto ev = loop->create_event("game.process_command",
	                             entity->get_manager(),
	                             state,
	                             // event is not executed until a command is available
	                             time::TIME_MAX,
	                             params);
	auto entity_queue = std::dynamic_pointer_cast<component::CommandQueue>(
		entity->get_component(component::component_t::COMMANDQUEUE));
	auto &queue = entity_queue->get_queue();
	queue.add_dependent(ev);

	return ev;
};

} // namespace openage::gamestate::activity
