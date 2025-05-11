// Copyright 2021-2025 the openage authors. See copying.md for legal info.

#include "command_queue.h"

#include <deque>

#include "gamestate/component/types.h"


namespace openage::gamestate::component {

CommandQueue::CommandQueue(const std::shared_ptr<openage::event::EventLoop> &loop) :
	command_queue{loop, 0},
	target{loop, 0} {
}

inline component_t CommandQueue::get_type() const {
	return component_t::COMMANDQUEUE;
}

void CommandQueue::add_command(const time::time_t &time,
                               const std::shared_ptr<command::Command> &command) {
	this->command_queue.insert(time, command);
}

curve::Queue<std::shared_ptr<command::Command>> &CommandQueue::get_queue() {
	return this->command_queue;
}

const std::shared_ptr<command::Command> CommandQueue::pop_command(const time::time_t &time) {
	return this->command_queue.pop_front(time);
}

curve::Discrete<CommandQueue::optional_target_t> &CommandQueue::get_target() {
	return this->target;
}

void CommandQueue::set_target(const time::time_t &time, const coord::phys3 &target) {
	this->target.set_last(time, target);
}

void CommandQueue::set_target(const time::time_t &time, const entity_id_t target) {
	this->target.set_last(time, target);
}

void CommandQueue::clear_target(const time::time_t &time) {
	this->target.set_last(time, std::nullopt);
}

} // namespace openage::gamestate::component
