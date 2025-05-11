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
	 * Adds a command to the queue.
	 *
	 * @param time Time at which the command is added.
	 * @param command New command.
	 */
	void add_command(const time::time_t &time,
	                 const std::shared_ptr<command::Command> &command);

	/**
	 * Get the command queue.
	 *
	 * @return Command queue.
	 */
	curve::Queue<std::shared_ptr<command::Command>> &get_queue();

	/**
	 * Get the command in the front of the queue.
	 *
	 * @param time Time at which the command is retrieved.
	 *
	 * @return Command in the front of the queue or nullptr if the queue is empty.
	 */
	const std::shared_ptr<command::Command> pop_command(const time::time_t &time);

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
	 * Get the targets of the entity over time.
	 *
	 * @return Targets over time.
	 */
	curve::Discrete<optional_target_t> &get_target();

private:
	/**
	 * Command queue.
	 */
	curve::Queue<std::shared_ptr<command::Command>> command_queue;

	/**
	 * Target of the entity.
	 */
	curve::Discrete<optional_target_t> target;
};

} // namespace gamestate::component
} // namespace openage
