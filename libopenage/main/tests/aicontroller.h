// Copyright 2019-2019 the openage authors. See copying.md for legal info.

#pragma once

#include "gamestate.h"

#include <vector>


namespace openage::main::tests::pong {

class AIInput {
public:
	const std::vector<PongEvent> &get_inputs(
		const std::shared_ptr<PongPlayer> &player,
		const std::shared_ptr<PongBall> &ball,
		const curve::time_t &now
	);

private:
	std::vector<PongEvent> commands;
};

} // openage::main::tests::pong
