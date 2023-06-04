// Copyright 2019-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate.h"
#include "../../curve/curve.h"

#include <vector>


namespace openage::event {

class EventLoop;

} // openage::event

namespace openage::main::tests::pong {

class Physics {
public:
	static void init(const std::shared_ptr<PongState> &,
	                 const std::shared_ptr<event::EventLoop> &mgr,
	                 const curve::time_t &);

	void process_input(const std::shared_ptr<PongState> &,
	                   const std::shared_ptr<PongPlayer> &,
	                   const std::vector<PongEvent> &input,
	                   const std::shared_ptr<event::EventLoop> &mgr,
	                   const curve::time_t &now);

	static void reset(const std::shared_ptr<event::State> &,
	                  event::EventLoop &mgr,
	                  const curve::time_t &);
};

} // openage::main::tests::pong
