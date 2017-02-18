// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "aicontroller.h"

namespace openage {
namespace tubepong {

std::vector<event> &AIInput::getInputs(
		const PongPlayer &player,
		const PongBall &ball,
		const tube::tube_time_t &now) {
	this->event_cache.clear();

	auto position = player.position.get(now);

	// Yes i know, there is /3 used - instead of the logical /2 - this is to
	// create a small safety boundary of 1/3 for enhanced fancyness

	// Ball is below position
	if (ball.position.get(now)[1] > position + player.size.get(now) / 3) {
		event_cache.push_back(event(player.id, event::DOWN));
	} else if (ball.position.get(now)[1] < position - player.size.get(now) / 3) {
		// Ball is above position
		event_cache.push_back(event(player.id, event::UP));
	}

	return this->event_cache;
}

}} // openage::tubepong
