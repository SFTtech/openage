// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "curve/queue.h"
#include "gamestate/component/internal_component.h"

namespace openage {

namespace event {
class EventLoop;
} // namespace event

namespace gamestate::component {

// TODO: Commands need payloads
enum class command_t {
	NONE,
	IDLE,
	MOVE,
};

class CommandQueue : public InternalComponent {
public:
	/**
	 * Creates an Ownership component.
	 *
	 * @param loop Event loop that all events from the component are registered on.
	 */
	CommandQueue(const std::shared_ptr<openage::event::EventLoop> &loop);

	component_t get_type() const override;

private:
	/**
	 * Command queue.
	 */
	curve::Queue<command_t> command_queue;
};

} // namespace gamestate::component
} // namespace openage
