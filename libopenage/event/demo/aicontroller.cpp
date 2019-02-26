// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#include "aicontroller.h"

namespace openage::event::demo {

std::vector<PongEvent> get_ai_inputs(const std::shared_ptr<PongPlayer> &player,
                                     const std::shared_ptr<PongBall> &ball,
                                     const curve::time_t &now) {
	std::vector<PongEvent> ret;

	auto position = player->position->get(now);

	// Yes i know, there is /3 used - instead of the logical /2 - this is to
	// create a small safety boundary of 1/3 for enhanced fancyness

	// Ball is below position
	if (ball->position->get(now)[1] > position + player->size->get(now) / 3) {
		ret.emplace_back(player->id(), PongEvent::DOWN);
	}
	// Ball is above position
	else if (ball->position->get(now)[1] < position - player->size->get(now) / 3) {
		ret.emplace_back(player->id(), PongEvent::UP);
	}
	else {
		ret.emplace_back(player->id(), PongEvent::IDLE);
	}

	return ret;
}

} // openage::event::demo
