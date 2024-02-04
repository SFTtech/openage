// Copyright 2018-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>


namespace openage::event {

class EventLoop;


class State {
public:
	State(const std::shared_ptr<EventLoop> & /*mgr*/) {}
	virtual ~State() = default;
};

} // namespace openage::event
