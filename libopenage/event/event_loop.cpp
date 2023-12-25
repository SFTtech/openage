// Copyright 2017-2023 the openage authors. See copying.md for legal info.

#include "event_loop.h"

#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>

#include "log/log.h"
#include "log/message.h"

#include "error/error.h"
#include "event/event.h"
#include "event/evententity.h"
#include "event/eventhandler.h"
#include "event/eventqueue.h"
#include "event/eventstore.h"
#include "util/fixed_point.h"


namespace openage::event {


void EventLoop::add_event_handler(const std::shared_ptr<EventHandler> eventhandler) {
	std::unique_lock lock{this->mutex};

	classstore.insert(std::make_pair(eventhandler->id(), eventhandler));
}


std::shared_ptr<Event> EventLoop::create_event(const std::string name,
                                               const std::shared_ptr<EventEntity> target,
                                               const std::shared_ptr<State> state,
                                               const time::time_t reference_time,
                                               const EventHandler::param_map params) {
	std::unique_lock lock{this->mutex};

	auto it = classstore.find(name);
	if (it == classstore.end()) {
		throw Error{MSG(err) << "Trying to subscribe to eventhandler "
		                     << name << ", which does not exist."};
	}

	return this->queue.create_event(target, it->second, state, reference_time, params);
}


std::shared_ptr<Event> EventLoop::create_event(const std::shared_ptr<EventHandler> eventhandler,
                                               const std::shared_ptr<EventEntity> target,
                                               const std::shared_ptr<State> state,
                                               const time::time_t reference_time,
                                               const EventHandler::param_map params) {
	std::unique_lock lock{this->mutex};

	auto it = this->classstore.find(eventhandler->id());
	if (it == this->classstore.end()) {
		auto res = this->classstore.insert(std::make_pair(eventhandler->id(), eventhandler));
		if (res.second) {
			it = res.first;
		}
		else {
			throw Error{ERR << "could not insert eventhandler into class store"};
		}
	}

	return this->queue.create_event(target, it->second, state, reference_time, params);
}


void EventLoop::reach_time(const time::time_t &time_until,
                           const std::shared_ptr<State> &state) {
	std::unique_lock lock{this->mutex};

	// TODO detect infinite loops (is this a halting problem?)
	// this happens when the events don't settle:
	// at least one processed event adds another event so
	// the queue never stops adding changes
	// simple "solution": abort after over 9000 attempts.
	int max_attempts = 10;

	int cnt = 0;
	int attempts = 0;

	do {
		if (attempts > max_attempts) {
			throw Error(ERR << "Loop: reached event settling threshold of "
			                << max_attempts << ", giving up.");
			break;
		}

		log::log(SPAM << "Loop: Attempt " << attempts << " to reach t=" << time_until);
		this->update_changes(state);
		cnt = this->execute_events(time_until, state);

		log::log(SPAM << "Loop: to reach t=" << time_until
		              << ", n=" << cnt << " events were executed");

		attempts += 1;
	}
	while (cnt != 0);

	// Swap in the end of the execution, else we might skip changes that happen
	// in the main loop for one frame - which is bad btw.
	this->queue.swap_changesets();
	log::log(SPAM << "Loop: t=" << time_until << " was reached! ========");
}


int EventLoop::execute_events(const time::time_t &time_until,
                              const std::shared_ptr<State> &state) {
	log::log(SPAM << "Loop: Pending events in the queue (# = "
	              << this->queue.get_event_queue().size() << "):");

	{
		size_t i = 0;
		for (const auto &e : this->queue.get_event_queue().get_sorted_events()) {
			log::log(SPAM << "  event "
			              << i << ": t=" << e->get_time() << ": " << e->get_eventhandler()->id());
			i++;
		}
	}

	int cnt = 0;

	while (true) {
		// fetch an event from the queue that happens before <= time_until
		std::shared_ptr<Event> event = this->queue.take_event(time_until);
		if (event == nullptr) {
			break;
		}

		auto target = event->get_entity().lock();

		if (target) {
			log::log(DBG << "Loop: invoking event \"" << event->get_eventhandler()->id()
			             << "\" on target \"" << target->idstr()
			             << "\" for time t=" << event->get_time());

			this->active_event = event;

			// apply the event effects
			event->get_eventhandler()->invoke(
				*this, target, state, event->get_time(), event->get_params());

			this->active_event = nullptr;
			cnt += 1;

			// if the event is REPEAT, readd the event.
			if (event->get_eventhandler()->type == EventHandler::trigger_type::REPEAT) {
				time::time_t new_time = event->get_eventhandler()->predict_invoke_time(
					target, state, event->get_time());

				if (new_time != time::TIME_MIN) {
					event->set_time(new_time);

					log::log(DBG << "Loop: repeating event \"" << event->get_eventhandler()->id()
					             << "\" on target \"" << target->idstr()
					             << "\" will be reenqueued for time t=" << event->get_time());

					this->queue.reenqueue(event);
				}
			}
		}
		else {
			// The element was already removed from the queue, so we can safely
			// kill it by ignoring it.
			log::log(DBG << "Loop: event \"" << event->get_eventhandler()->id()
			             << "\" ignored because its target does not exist anymore "
			             << "\" for time t=" << event->get_time());
		}
	}
	return cnt;
}


void EventLoop::create_change(const std::shared_ptr<Event> evnt,
                              const time::time_t changes_at) {
	std::unique_lock lock{this->mutex};

	this->queue.add_change(evnt, changes_at);
}


void EventLoop::update_changes(const std::shared_ptr<State> &state) {
	log::log(SPAM << "Loop: " << this->queue.get_changes().size()
	              << " target changes have to be processed");

	size_t i = 0;

	// Some EventEntity has changed, so all depending events were
	// added to the EventQueue as changes.
	// These changes need to be reevaluated.
	for (const auto &change : this->queue.get_changes()) {
		auto evnt = change.evnt.lock();
		if (evnt) {
			log::log(DBG << "  change " << i++ << ": " << evnt->get_eventhandler()->id());
			switch (evnt->get_eventhandler()->type) {
			case EventHandler::trigger_type::ONCE:
			case EventHandler::trigger_type::DEPENDENCY: {
				auto entity = evnt->get_entity().lock();

				if (entity) {
					time::time_t new_time = evnt->get_eventhandler()
					                            ->predict_invoke_time(entity, state, change.time);

					if (new_time != time::TIME_MIN) {
						log::log(DBG << "Loop: due to a change, rescheduling event of '"
						             << evnt->get_eventhandler()->id()
						             << "' on entity '" << entity->idstr()
						             << "' at time t=" << change.time
						             << " to NEW TIME t=" << new_time);

						evnt->set_time(new_time);

						this->queue.enqueue(evnt);
					}
					else {
						log::log(DBG << "Loop: due to a change, canceled execution of '"
						             << evnt->get_eventhandler()->id()
						             << "' on entity '" << entity->idstr()
						             << "' at time t=" << change.time);

						this->queue.remove(evnt);
					}
				}
				else {
					// the event is for a no-longer-existing entity,
					// so we can remove it from the queue.
					this->queue.remove(evnt);
				}
			} break;

			case EventHandler::trigger_type::TRIGGER:
			case EventHandler::trigger_type::DEPENDENCY_IMMEDIATELY:
				evnt->set_time(change.time);
				this->queue.enqueue(evnt);
				break;

			case EventHandler::trigger_type::REPEAT:
				break;
			}
		}
	}

	this->queue.clear_changes();
}

} // namespace openage::event
