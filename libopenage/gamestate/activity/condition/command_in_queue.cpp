// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#include "next_command.h"

#include "gamestate/component/internal/command_queue.h"
#include "gamestate/game_entity.h"


namespace openage::gamestate::activity {

bool command_in_queue(const time::time_t &time,
                      const std::shared_ptr<gamestate::GameEntity> &entity,
                      const std::shared_ptr<gamestate::GameState> & /* state */,
                      const std::shared_ptr<nyan::Object> & /* api_object */) {
	auto command_queue = std::dynamic_pointer_cast<component::CommandQueue>(
		entity->get_component(component::component_t::COMMANDQUEUE));

	return not command_queue->get_commands().empty(time);
}

} // namespace openage::gamestate::activity
