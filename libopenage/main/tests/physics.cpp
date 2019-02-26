// Copyright 2019-2019 the openage authors. See copying.md for legal info.

#include "physics.h"

#include <cmath>

#include "gui.h"
#include "../../error/error.h"
#include "../../rng/global_rng.h"
#include "../../util/stringformatter.h"


namespace openage::main::tests::pong {


class BallReflectWall : public event::DependencyEventClass {
public:
	BallReflectWall() : event::DependencyEventClass("demo.ball.reflect_wall") {}

	void setup_event(const std::shared_ptr<event::Event> &evnt,
	                 const std::shared_ptr<event::State> &gstate) override {

		auto state = std::dynamic_pointer_cast<PongState>(gstate);

		// FIXME dependency to a full ball object so that any curve
		// triggers a change
		evnt->depend_on(state->ball->position);
		evnt->depend_on(state->ball->speed);
		// TODO add dependency to size of game area

		// FIXME: warn if it's not a dependency eventclass
	}

	// FIXME we REALLY need dependencies to objects i.e. Ball : public EventTarget()
	void invoke(event::Loop &,
	            const std::shared_ptr<event::EventTarget> &target,
	            const std::shared_ptr<event::State> &gstate,
	            const curve::time_t &now,
	            const event::EventClass::param_map &/*param*/) override {

		auto positioncurve = std::dynamic_pointer_cast<curve::Continuous<util::Vector2d>>(target);
		auto state = std::dynamic_pointer_cast<PongState>(gstate);
		auto speedcurve = state->ball->speed;

		// get speed and position to insert new movement keyframe
		auto speed = speedcurve->get(now);
		auto pos = positioncurve->get(now);
		auto screen_size = state->area_size->get(now);

		speed[1] *= -1.0;
		state->ball->speed->set_last(now, speed);
		state->ball->position->set_last(now, pos);

		if (speed[1] == 0) {
			return;
		}

		curve::time_t ty = 0;
		if (speed[1] > 0) {
			ty = curve::time_t::from_double(
				(screen_size[1] - pos[1]) / speed[1]
			);
		}
		else if (speed[1] < 0) {
			ty = curve::time_t::from_double(
				pos[1] / -speed[1]
			);
		}

		state->ball->position->set_last(now + ty, pos + (speed * ty.to_double()));
	}

	curve::time_t predict_invoke_time(const std::shared_ptr<event::EventTarget> &target,
	                                  const std::shared_ptr<event::State> &gstate,
	                                  const curve::time_t &now) override {

		auto positioncurve = std::dynamic_pointer_cast<curve::Continuous<util::Vector2d>>(target);
		auto state = std::dynamic_pointer_cast<PongState>(gstate);

		auto speed = state->ball->speed->get(now);
		auto pos = positioncurve->get(now);
		auto screen_size = state->area_size->get(now);

		if (speed[1] == 0) {
			return std::numeric_limits<curve::time_t>::max();
		}

		curve::time_t ty = 0;
		if (speed[1] > 0) {
			ty = curve::time_t::from_double((screen_size[1] - pos[1]) / speed[1]);
		}
		else if (speed[1] < 0) {
			ty = curve::time_t::from_double(pos[1] / -speed[1]);
		}

		util::FString str;
		str.fmt("WALL TY %f NOW %f, NOWTY %f ",
		        ty.to_double(), now.to_double(), (now + ty).to_double());
		state->gui->log(str);

		return now + ty;
	}
};


class BallReflectPanel : public event::DependencyEventClass {
public:
	BallReflectPanel ()
		:
		event::DependencyEventClass("demo.ball.reflect_panel") {}

	void setup_event(const std::shared_ptr<event::Event> &target,
	                 const std::shared_ptr<event::State> &gstate) override {

		auto state = std::dynamic_pointer_cast<PongState>(gstate);

		// FIXME dependency to a full ball object
		// then a change to any of the curves triggers the update.
		target->depend_on(state->ball->position);
		target->depend_on(state->ball->speed);
		target->depend_on(state->p1->position);
		target->depend_on(state->p2->position);
		// TODO add dependency to size of game area
	}

	// FIXME we REALLY need dependencies to objects
	void invoke(event::Loop &mgr,
	            const std::shared_ptr<event::EventTarget> &/*target*/,
	            const std::shared_ptr<event::State> &gstate,
	            const curve::time_t &now,
	            const event::EventClass::param_map &/*param*/) override {

		auto state = std::dynamic_pointer_cast<PongState>(gstate);

		auto pos = state->ball->position->get(now);
		auto speed = state->ball->speed->get(now);
		auto screen_size = state->area_size->get(now);

		static int cnt = 0;
		util::FString str;
		str.fmt("Panel hit [%i]", ++cnt);
		state->gui->log(str);

		if (pos[0] <= 1 and
		    speed[0] < 0 and
		    (pos[1] < state->p1->position->get(now) - state->p1->size->get(now) / 2 or
		     pos[1] > state->p1->position->get(now) + state->p1->size->get(now) / 2)) {

			// ball missed the paddle of player 1
			auto l = state->p1->lives->get(now);
			l--;

			state->p1->lives->set_last(now, l);
			state->ball->position->set_last(now, pos);

			Physics::reset(state, mgr, now);
		}
		else if (pos[0] >= screen_size[0] - 1 and
		         speed[0] > 0 and
		         (pos[1] < state->p2->position->get(now) - state->p2->size->get(now) / 2 or
		          pos[1] > state->p2->position->get(now) + state->p2->size->get(now) / 2)) {

			// ball missed the paddel of player 2
			auto l = state->p2->lives->get(now);
			l--;
			state->p2->lives->set_last(now, l);
			state->ball->position->set_last(now, pos);

			Physics::reset(state, mgr, now);
		}
		else if (pos[0] >= screen_size[0]- 1 || pos[0] <= 1) {
			speed[0] *= -1;
			state->ball->speed->set_last(now, speed);
			state->ball->position->set_last(now, pos);
		}

		curve::time_t ty = 0;
		if (speed[0] > 0) {
			ty = curve::time_t::from_double((screen_size[0] - pos[0]) / speed[0]);
		}
		else if (speed[0] < 0) {
			ty = curve::time_t::from_double(pos[0] / -speed[0]);
		}

		auto hit_pos = pos + speed * ty.to_double();
		if (speed[0] > 0) {
			hit_pos[0] = screen_size[0];
		}
		else {
			hit_pos[0] = 0;
		}

		state->ball->position->set_last(now + ty, hit_pos);
	}

	curve::time_t predict_invoke_time(const std::shared_ptr<event::EventTarget> &target,
	                                  const std::shared_ptr<event::State> &gstate,
	                                  const curve::time_t &now) override {

		auto positioncurve = std::dynamic_pointer_cast<curve::Continuous<util::Vector2d>>(target);
		auto state = std::dynamic_pointer_cast<PongState>(gstate);

		auto speed = state->ball->speed->get(now);
		auto pos = positioncurve->get(now);
		auto screen_size = state->area_size->get(now);

		if (speed[0] == 0)
			return std::numeric_limits<curve::time_t>::max();

		curve::time_t ty = 0;

		if (speed[0] > 0) {
			ty = curve::time_t::from_double((screen_size[0] - pos[0]) / speed[0]);
		}
		else if (speed[0] < 0) {
			ty = curve::time_t::from_double(pos[0] / -speed[0]);
		}

		{
			util::FString str;
			str.fmt("PANEL REFLECT AT %f NEXT %f", now.to_double(), (now + ty).to_double());
			state->gui->log(str);
		}

		auto hit_pos = pos + speed * ty.to_double();
		if (speed[0] > 0) {
			hit_pos[0] = screen_size[0];
		}
		else {
			hit_pos[0] = 0;
		}

		ENSURE(hit_pos[0] >= 0, "hit position x must be positive");

		return now + ty;
	}
};


class ResetGame : public event::OnceEventClass {
public:
	ResetGame ()
		:
		event::OnceEventClass("demo.reset") {}

	void setup_event(const std::shared_ptr<event::Event> &/*target*/,
	                 const std::shared_ptr<event::State> &/*state*/) override {}

	void invoke(event::Loop &/*mgr*/,
	            const std::shared_ptr<event::EventTarget> &/*target*/,
	            const std::shared_ptr<event::State> &gstate,
	            const curve::time_t &now,
	            const event::EventClass::param_map &/*param*/) override {

		auto state = std::dynamic_pointer_cast<PongState>(gstate);

		auto screen_size = state->area_size->get(now);

		// Check if the condition still applies
		{
			auto pos = state->ball->position->get(now);
			if (pos[0] > 0 && pos[0] < screen_size[0]) {
				// the gamestate is still valid - there is no need to reset
				throw Error(ERR << "reset invoked even though unnecessary");
			}
		}

		// move ball to the center, quickly
		state->ball->position->set_last(now - curve::time_t::from_double(0.01),
		                                state->ball->position->get(now));
		state->ball->position->set_last(now, screen_size.casted<double>() / 2);

		// move paddles to center
		state->p1->position->set_last(now, screen_size[1] / 2);
		state->p2->position->set_last(now, screen_size[1] / 2);

		// initial speed of the ball:
		float dirx = (rng::random() % 2) ? 1 : -1;
		float diry = (rng::random() % 2) ? 1 : -1;
		auto init_speed = util::Vector2d(
			dirx * (screen_size[0] / 2.0 + (rng::random() % (screen_size[0]/4))),
			diry * (screen_size[1] / 3.0 + (rng::random() % (screen_size[1]/5)))
		);

		state->ball->speed->set_last(now, init_speed);
		auto pos = state->ball->position->get(now);

		{
			static int cnt = 0;

			util::FString str;
			str.fmt("Reset #%i. Speed %f | %f POS %f | %f",
			        ++cnt,
			        init_speed[0],
			        init_speed[1],
			        pos[0],
			        pos[1]);
			state->gui->log(str);
		}

		curve::time_t ty = 0;

		// calculate the wall-hit-times
		if (init_speed[1] > 0) {
			ty = curve::time_t::from_double((screen_size[1] - pos[1]) / init_speed[1]);
		}
		else if (init_speed[1] < 0) {
			ty = curve::time_t::from_double(pos[1] / -init_speed[1]);
		}
		else {
			// currently never happens, but this would be a non-vertically-moving ball
			// fallback to calculating panel-hit-times
			if (init_speed[0] > 0) {
				ty = curve::time_t::from_double((screen_size[0] - pos[0]) / init_speed[0]);
			}
			else {
				ty = curve::time_t::from_double(pos[0] / -init_speed[0]);
			}
		}

		state->ball->position->set_last(now + ty, pos + init_speed * ty.to_double());
	}

	curve::time_t predict_invoke_time(const std::shared_ptr<event::EventTarget> &/*target*/,
	                                  const std::shared_ptr<event::State> &/*state*/,
	                                  const curve::time_t &old_time) override {
		return old_time;
	}
};


void Physics::init(const std::shared_ptr<PongState> &state,
                   const std::shared_ptr<event::Loop> &loop,
                   const curve::time_t &now) {

	loop->add_event_class(std::make_shared<BallReflectPanel>());
	loop->add_event_class(std::make_shared<BallReflectWall>());
	loop->add_event_class(std::make_shared<ResetGame>());

	loop->create_event("demo.ball.reflect_wall", state->ball->position, state, now);
	loop->create_event("demo.ball.reflect_panel", state->ball->position, state, now);

	// FIXME once "reset": deregister
	//reset(state, mgr, now);
}


void Physics::reset(const std::shared_ptr<event::State> &gstate,
                    event::Loop &mgr,
                    const curve::time_t &now) {

	auto state = std::dynamic_pointer_cast<PongState>(gstate);
	mgr.create_event("demo.reset", state->ball->position, state, now);
}


void Physics::process_input(const std::shared_ptr<PongState> &state,
                            const std::shared_ptr<PongPlayer> &player,
                            const std::vector<PongEvent> &events,
                            const std::shared_ptr<event::Loop> &mgr,
                            const curve::time_t &now) {

	// seconds into the future
	constexpr static auto predicted_movement_time = curve::time_t::from_double(5.0);

	// pixels per second for paddle movement
	constexpr static double movement_speed = 350.0;

	for (auto& evnt : events) {

		// Process only if the future has changed
		if (player->state->get(now).state != evnt.state) {

			// log the new input in the state curve
			player->state->set_last(now, evnt);

			auto screen_size = state->area_size->get(now);

			// if the state is active longer than predicted,
			// we have to extend the prediction!
			bool extend_previous_prediction = false;
			auto prev_state = player->state->get_previous(now);
			if (prev_state and prev_state->second.state == evnt.state) {
				extend_previous_prediction = true;
			}

			switch(evnt.state) {
			case PongEvent::UP:
			case PongEvent::DOWN: {
				float current_pos = player->position->get(now);

				if (not extend_previous_prediction) {
					// create a keyframe for the new movement
					player->position->set_last(now, current_pos);
					// TODO: drop the intermediate keyframes
					//       for position and speed.
				}

				switch (evnt.state) {
				case PongEvent::UP:
					player->speed->set_last(now, -movement_speed);
					break;
				case PongEvent::DOWN:
					player->speed->set_last(now, movement_speed);
					break;
				default:
					// never reached.
					break;
				}

				curve::time_t move_stop_guess = now + predicted_movement_time;

				// change the position by integrating the speed curve.
				// TODO: add native integral to curves.
				float new_pos = current_pos +
				                (((player->speed->get(move_stop_guess) +
				                   player->speed->get(now)) / 2.0) *
				                 predicted_movement_time.to_double());

				// if paddle will get out-of-bounds, calculate the bound hit time

				// TODO: add native key/value finding in range to curves
				if (new_pos < 0) {
					move_stop_guess = now + (current_pos / movement_speed);
					new_pos = 0;
				}

				if (new_pos > screen_size[1]) {
					move_stop_guess = now + ((screen_size[1] - current_pos) / movement_speed);
					new_pos = screen_size[1];
				}

				PongEvent set_idle{evnt.player, PongEvent::IDLE};
				player->state->set_last(move_stop_guess, set_idle);
				player->speed->set_last(move_stop_guess, 0);
				player->position->set_last(move_stop_guess, new_pos);

				// TODO: predict_reflect_panel(state, queue, now);
				break;
			}

			case PongEvent::IDLE:
				player->position->set_last(now, player->position->get(now));
				player->speed->set_last(now, 0);
				break;

			case PongEvent::START:
				Physics::reset(state, *mgr, now);
				break;

			default:
				break;
			}
		}
	}
}

} // openage::main::tests::pong
