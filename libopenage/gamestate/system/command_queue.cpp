// Copyright 2025-2025 the openage authors. See copying.md for legal info.

#include "command_queue.h"

#include "gamestate/game_entity.h"
#include "gamestate/component/internal/command_queue.h"
#include "gamestate/component/types.h"


namespace openage::gamestate::system {

const time::time_t CommandQueue::clear_queue(const std::shared_ptr<gamestate::GameEntity> &entity,
                                              const time::time_t &start_time) {
    auto command_queue = std::dynamic_pointer_cast<component::CommandQueue>(
        entity->get_component(component::component_t::COMMANDQUEUE));

    // Clear the queue
    auto &queue = command_queue->get_queue();
    queue.clear(start_time);

    // Clear the target
    command_queue->clear_target(start_time);

    return start_time;
}

const time::time_t CommandQueue::pop_command(const std::shared_ptr<gamestate::GameEntity> &entity,
                                              const time::time_t &start_time) {
    auto command_queue = std::dynamic_pointer_cast<component::CommandQueue>(
        entity->get_component(component::component_t::COMMANDQUEUE));

    // Pop the command
    auto &queue = command_queue->get_queue();
    queue.pop_front(start_time);

    return start_time;
}

} // namespace
