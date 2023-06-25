// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "curve/queue.h"
#include "gamestate/component/internal/commands/base_command.h"
#include "gamestate/component/internal_component.h"

namespace openage {

namespace event {
class EventLoop;
} // namespace event

namespace gamestate::component {

class CommandQueue : public InternalComponent {
public:
	/**
	 * Creates an Ownership component.
	 *
	 * @param loop Event loop that all events from the component are registered on.
	 */
	CommandQueue(const std::shared_ptr<openage::event::EventLoop> &loop);

	component_t get_type() const override;

	/**
	 * Adds a command to the queue.
	 *
	 * @param time Time at which the command is added.
	 * @param command New command.
	 */
	void add_command(const curve::time_t &time,
	                 std::shared_ptr<command::Command> command);

private:
	/**
	 * Command queue.
	 */
	curve::Queue<std::shared_ptr<command::Command>> command_queue;
};

} // namespace gamestate::component
} // namespace openage
