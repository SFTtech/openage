// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#include "next_command_switch.h"

#include "gamestate/component/internal/command_queue.h"
#include "gamestate/game_entity.h"


namespace openage::gamestate::activity {

int next_command_switch(const time::time_t &time,
                        const std::shared_ptr<gamestate::GameEntity> &entity,
                        const std::shared_ptr<nyan::Object> & /* api_object */) {
	auto command_queue = std::dynamic_pointer_cast<component::CommandQueue>(
		entity->get_component(component::component_t::COMMANDQUEUE));

	if (command_queue->get_queue().empty(time)) {
		return -1;
	}

	auto command = command_queue->get_queue().front(time);

	return static_cast<int>(command->get_type());
}

} // namespace openage::gamestate::activity
