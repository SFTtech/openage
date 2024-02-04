// Copyright 2019-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "main/demo/pong/gamestate.h"


namespace openage::main::tests::pong {

std::vector<PongEvent> get_ai_inputs(
	const std::shared_ptr<PongPlayer> &player,
	const std::shared_ptr<PongBall> &ball,
	const std::shared_ptr<curve::Discrete<util::Vector2s>> &area_size,
	const time::time_t &now,
	bool right_player);

} // namespace openage::main::tests::pong
