// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate.h"
#include "../../curve/curve.h"

#include <vector>


namespace openage::event {

class Loop;

namespace demo {

class Physics {
public:
	static void init(const std::shared_ptr<PongState> &,
	                 const std::shared_ptr<Loop> &mgr,
	                 const curve::time_t &);

	void process_input(const std::shared_ptr<PongState> &,
	                   const std::shared_ptr<PongPlayer> &,
	                   const std::vector<PongEvent> &input,
	                   const std::shared_ptr<Loop> &mgr,
	                   const curve::time_t &now);

	static void reset(const std::shared_ptr<State> &,
	                  Loop &mgr,
	                  const curve::time_t &);
};

}} // openage::event::demo
