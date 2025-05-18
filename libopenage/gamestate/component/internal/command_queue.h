// Copyright 2021-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <variant>

#include "coord/phys.h"
#include "curve/container/queue.h"
#include "curve/discrete.h"
#include "gamestate/component/internal/commands/base_command.h"
#include "gamestate/component/internal_component.h"
#include "gamestate/component/types.h"
#include "gamestate/types.h"
#include "time/time.h"


namespace openage {

namespace event {
class EventLoop;
} // namespace event

namespace gamestate::component {

class CommandQueue final : public InternalComponent {
public:
	/**
	 * Creates an Ownership component.
	 *
	 * @param loop Event loop that all events from the component are registered on.
	 */
	CommandQueue(const std::shared_ptr<openage::event::EventLoop> &loop);

	component_t get_type() const override;

	/**
	 * Append a command to the queue.
	 *
	 * @param time Time at which the command is appended.
	 * @param command New command.
	 */
	void add_command(const time::time_t &time,
	                 const std::shared_ptr<command::Command> &command);

	/**
	 * Clear the queue and set the front command.
	 *
	 * @param time Time at which the command is set.
	 * @param command New command.
	 */
	void set_command(const time::time_t &time,
	                 const std::shared_ptr<command::Command> &command);

	/**
	 * Get the command queue.
	 *
	 * @return Command queue.
	 */
	const curve::Queue<std::shared_ptr<command::Command>> &get_commands();

	/**
	 * Clear all commands in the queue.
	 *
	 * @param time Time at which the queue is cleared.
	 */
	void clear(const time::time_t &time);

	/**
	 * Get the command in the front of the queue and remove it.
	 *
	 * Unlike curve::Queue::front(), calling this on an empty queue is
	 * not undefined behavior.
	 *
	 * @param time Time at which the command is popped.
	 *
	 * @return Command in the front of the queue or nullptr if the queue is empty.
	 */
	const std::shared_ptr<command::Command> pop(const time::time_t &time);

	/**
	 * get the command at the front of the queue.
	 *
	 * @param time Time at which the command is retrieved.
	 *
	 * @return Command in the front of the queue or nullptr if the queue is empty.
	 */
	const std::shared_ptr<command::Command> front(const time::time_t &time) const;

	/**
	 * Target type with several possible representations.
	 *
	 * Can be:
	 * - coord::phys3: Position in the game world.
	 * - entity_id_t: ID of another entity.
	 * - std::nullopt: Nothing.
	 */
	using optional_target_t = std::optional<std::variant<coord::phys3, entity_id_t>>;

	/**
	 * Get the target of the entity at the given time.
	 *
	 * The target may be empty if the command queue is empty or if the command
	 * has no target.
	 *
	 * @return Target of the entity.
	 */
	optional_target_t get_target(const time::time_t &time) const;

private:
	/**
	 * Command queue.
	 */
	curve::Queue<std::shared_ptr<command::Command>> command_queue;
};

} // namespace gamestate::component
} // namespace openage
