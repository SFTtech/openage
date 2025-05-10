// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#include "next_command.h"

#include <nyan/nyan.h>

#include "gamestate/api/definitions.h"
#include "gamestate/component/internal/command_queue.h"
#include "gamestate/game_entity.h"


namespace openage::gamestate::activity {

bool next_command(const time::time_t &time,
                  const std::shared_ptr<gamestate::GameEntity> &entity,
                  const std::shared_ptr<nyan::Object> &condition) {
	auto command_queue = std::dynamic_pointer_cast<component::CommandQueue>(
		entity->get_component(component::component_t::COMMANDQUEUE));

	if (command_queue->get_queue().empty(time)) {
		return false;
	}

	auto queue_command = command_queue->get_queue().front(time);

	auto compare_command = condition->get<nyan::ObjectValue>("NextCommand.command");
	auto compare_type = api::COMMAND_DEFS.get(compare_command->get_name());

	return queue_command->get_type() == compare_type;
}

} // namespace openage::gamestate::activity
