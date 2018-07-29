// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#include "aicontroller.h"

namespace openage::event::demo {

const std::vector<PongEvent> &AIInput::get_inputs(const std::shared_ptr<PongPlayer> &player,
                                                  const std::shared_ptr<PongBall> &ball,
                                                  const curve::time_t &now) {
	this->commands.clear();

	auto position = player->position->get(now);

	// Yes i know, there is /3 used - instead of the logical /2 - this is to
	// create a small safety boundary of 1/3 for enhanced fancyness

	// Ball is below position
	if (ball->position->get(now)[1] > position + player->size->get(now) / 3) {
		this->commands.push_back(PongEvent{player->id(), PongEvent::DOWN});
	}
	// Ball is above position
	else if (ball->position->get(now)[1] < position - player->size->get(now) / 3) {
		this->commands.push_back(PongEvent{player->id(), PongEvent::UP});
	}

	return this->commands;
}

} // openage::event::demo
