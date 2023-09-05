// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <string>

#include "event/evententity.h"
#include "event/eventhandler.h"


namespace openage {

namespace event {
class EventLoop;
class Event;
class State;
} // namespace event

namespace gamestate::event {

// TODO: This is only for testing
class Commander : public openage::event::EventEntity {
public:
	Commander(const std::shared_ptr<openage::event::EventLoop> &loop);
	~Commander() = default;

	size_t id() const override;
	std::string idstr() const override;
};

/**
 * Send commands to game entities.
 */
class SendCommandHandler : public openage::event::OnceEventHandler {
public:
	SendCommandHandler();
	~SendCommandHandler() = default;

	void setup_event(const std::shared_ptr<openage::event::Event> &event,
	                 const std::shared_ptr<openage::event::State> &state) override;

	void invoke(openage::event::EventLoop &loop,
	            const std::shared_ptr<openage::event::EventEntity> &target,
	            const std::shared_ptr<openage::event::State> &state,
	            const time::time_t &time,
	            const param_map &params) override;

	time::time_t predict_invoke_time(const std::shared_ptr<openage::event::EventEntity> &target,
	                                 const std::shared_ptr<openage::event::State> &state,
	                                 const time::time_t &at) override;
};

} // namespace gamestate::event
} // namespace openage
