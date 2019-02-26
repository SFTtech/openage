// Copyright 2019-2019 the openage authors. See copying.md for legal info.

#include "aicontroller.h"


namespace openage::main::tests::pong {


std::vector<PongEvent> get_ai_inputs(const std::shared_ptr<PongPlayer> &player,
                                     const std::shared_ptr<PongBall> &ball,
                                     const std::shared_ptr<curve::Discrete<util::Vector2s>> &area_size_curve,
                                     const curve::time_t &now,
                                     bool right_player) {
	std::vector<PongEvent> ret;

	auto player_pos = player->position->get(now);
	auto player_size = player->size->get(now);
	auto ball_pos = ball->position->get(now);
	auto speed = ball->speed->get(now);

	auto area_size = area_size_curve->get(now);
	double area_width = area_size[0];
	double area_height = area_size[1];

	curve::time_t hit_time;
	util::Vector2d hit_pos;

	// calculate ball trajectory
	// extrapolate trajectory to player's panel wall
	// follow reflections at walls
	// move panel to predicted hit position of panel wall

	while (true) {
		curve::time_t ty_hit = 0, tx_hit = 0;

		if (speed[0] == 0) {
			tx_hit = std::numeric_limits<curve::time_t>::max();
		} else if (speed[0] > 0) {
			tx_hit = curve::time_t::from_double((area_width - ball_pos[0]) / speed[0]);
		} else if (speed[0] < 0) {
			tx_hit = curve::time_t::from_double(ball_pos[0] / -speed[0]);
		}

		if (speed[1] == 0) {
			ty_hit = std::numeric_limits<curve::time_t>::max();
		} else if (speed[1] > 0) {
			ty_hit = curve::time_t::from_double((area_height - ball_pos[1]) / speed[1]);
		} else if (speed[1] < 0) {
			ty_hit = curve::time_t::from_double(ball_pos[1] / -speed[1]);
		}

		// actual hit has lowest time:
		hit_time = std::min(ty_hit, tx_hit);
		hit_pos = ball_pos + (speed * hit_time.to_double());

		// continue calculating until panel hit
		if (ty_hit < tx_hit) {
			speed[1] *= -1;
			ball_pos = hit_pos;
			continue;
		}
		else {
			// stop the iteration if the ball is moving towards us
			// i.e. it is not hitting the panel on the oposite wall.
			if (right_player and speed[0] > 0) {
				break;
			}
			else if (not right_player and speed[0] < 0) {
				break;
			}

			speed[0] *= -1;
			ball_pos = hit_pos;
			continue;
		}
	}

	// move the panel towards the ball hit pos, but only
	// if the ball does not hit the middle piece of the 3-divided panel
	if (hit_pos[1] > player_pos + (player_size / 3)) {
		ret.push_back(PongEvent{player->id(), PongEvent::DOWN});
	}
	else if (hit_pos[1] < player_pos - (player_size / 3)) {
		ret.push_back(PongEvent{player->id(), PongEvent::UP});
	}
	else {
		ret.push_back(PongEvent{player->id(), PongEvent::IDLE});
	}

	return ret;
}

} // openage::main::tests::pong
