// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "config.h"
#include "gamestate.h"

#include "../curve.h"
#include "../events/eventmanager.h"

#include <vector>

namespace openage {
namespace curvepong {

class Physics {
public:
	static void init(std::shared_ptr<State> &,
	                 curve::EventManager *,
	                 const curve::curve_time_t &);

	void processInput(std::shared_ptr<State> &,
	                  std::shared_ptr<PongPlayer> &,
	                  std::vector<event> &input,
	                  curve::EventManager *,
	                  const curve::curve_time_t &now);

	static void reset(const std::shared_ptr<State> &,
	                  curve::EventManager *mgr,
	                  const curve::curve_time_t &);
};

}} // openage::curvepong
