// Copyright 2021-2025 the openage authors. See copying.md for legal info.

#include "command_queue.h"

#include <deque>

#include "gamestate/component/internal/commands/apply_effect.h"
#include "gamestate/component/internal/commands/move.h"
#include "gamestate/component/types.h"


namespace openage::gamestate::component {

CommandQueue::CommandQueue(const std::shared_ptr<openage::event::EventLoop> &loop) :
	command_queue{loop, 0} {
}

inline component_t CommandQueue::get_type() const {
	return component_t::COMMANDQUEUE;
}

void CommandQueue::add_command(const time::time_t &time,
                               const std::shared_ptr<command::Command> &command) {
	this->command_queue.insert(time, command);
}

const curve::Queue<std::shared_ptr<command::Command>> &CommandQueue::get_commands() {
	return this->command_queue;
}

void CommandQueue::clear(const time::time_t &time) {
	this->command_queue.clear(time);
}

const std::shared_ptr<command::Command> CommandQueue::pop(const time::time_t &time) {
	if (this->command_queue.empty(time)) {
		return nullptr;
	}

	return this->command_queue.pop_front(time);
}

const std::shared_ptr<command::Command> CommandQueue::front(const time::time_t &time) const {
	if (this->command_queue.empty(time)) {
		return nullptr;
	}

	return this->command_queue.front(time);
}

CommandQueue::optional_target_t CommandQueue::get_target(const time::time_t &time) const {
	if (this->command_queue.empty(time)) {
		return std::nullopt;
	}

	auto command = this->command_queue.front(time);

	// Extract the target from the command
	switch (command->get_type()) {
	case command::command_t::MOVE:
		return std::dynamic_pointer_cast<command::Move>(command)->get_target();
	case command::command_t::APPLY_EFFECT:
		return std::dynamic_pointer_cast<command::ApplyEffect>(command)->get_target();
	default:
		return std::nullopt;
	}
}

} // namespace openage::gamestate::component
