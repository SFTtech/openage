// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

#include "time/time.h"


namespace openage::event {

class EventLoop;
class State;

namespace demo {

class PongEvent;
class PongPlayer;
class PongState;

class Physics {
public:
	static void init(const std::shared_ptr<PongState> &,
	                 const std::shared_ptr<EventLoop> &mgr,
	                 const time::time_t &);

	void process_input(const std::shared_ptr<PongState> &,
	                   const std::shared_ptr<PongPlayer> &,
	                   const std::vector<PongEvent> &input,
	                   const std::shared_ptr<EventLoop> &mgr,
	                   const time::time_t &now);

	static void reset(const std::shared_ptr<State> &,
	                  EventLoop &mgr,
	                  const time::time_t &);
};

} // namespace demo
} // namespace openage::event
