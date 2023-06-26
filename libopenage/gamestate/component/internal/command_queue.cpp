// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "command_queue.h"

#include "gamestate/component/types.h"


namespace openage::gamestate::component {

CommandQueue::CommandQueue(const std::shared_ptr<openage::event::EventLoop> &loop) :
	command_queue{loop, 0} {
}

inline component_t CommandQueue::get_type() const {
	return component_t::COMMANDQUEUE;
}

void CommandQueue::add_command(const curve::time_t &time,
                               std::shared_ptr<command::Command> command) {
	this->command_queue.insert(time, command);
}

const curve::Queue<std::shared_ptr<command::Command>> &CommandQueue::get_queue() const {
	return this->command_queue;
}


} // namespace openage::gamestate::component
