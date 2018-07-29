// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#include "loop.h"

#include "event.h"
#include "eventclass.h"
#include "eventqueue.h"
#include "eventtarget.h"

#include "../log/log.h"


namespace openage::event {


void Loop::add_event_class(const std::shared_ptr<EventClass> &cls) {
	classstore.insert(std::make_pair(cls->id(), cls));
}


std::weak_ptr<Event> Loop::create_event(const std::string &name,
                                        const std::shared_ptr<EventTarget> &target,
                                        const std::shared_ptr<State> &state,
                                        const curve::time_t &reference_time,
                                        const EventClass::param_map &params) {
	auto it = classstore.find(name);
	if (it == classstore.end()) {
		throw Error{MSG(err) << "Trying to subscribe to eventclass "
		                     << name << ", which does not exist."};
	}

	return this->queue.create_event(target, it->second, state, reference_time, params);
}


std::weak_ptr<Event> Loop::create_event(const std::shared_ptr<EventClass> &eventclass,
                                        const std::shared_ptr<EventTarget> &target,
                                        const std::shared_ptr<State> &state,
                                        const curve::time_t &reference_time,
                                        const EventClass::param_map &params) {

	auto it = this->classstore.find(eventclass->id());
	if (it == this->classstore.end()) {
		auto res = this->classstore.insert(std::make_pair(eventclass->id(), eventclass));
		if (res.second) {
			it = res.first;
		} else {
			throw Error{ERR << "could not insert eventclass into class store"};
		}
	}

	return this->queue.create_event(target, it->second, state, reference_time, params);
}


void Loop::reach_time(const curve::time_t &max_time,
                      const std::shared_ptr<State> &state) {

	// TODO detect infinite loops (is this a halting problem?)
	// this happens when the events don't settle:
	// at least one processed event adds another event so
	// the queue never stops adding changes
	// simple "solution": abort after over 9000 attempts.

	int cnt = 0;
	int attempts = 0;
	do {
		log::log(DBG << "Loop: Attempt " << attempts << " to reach t=" << max_time);
		this->update_changes(state);
		cnt = this->execute_events(max_time, state);

		log::log(DBG << "Loop: to reach t=" << max_time
		         << ", n=" << cnt << " events were executed");

		if (attempts > 9000) {
			log::log(WARN << "Loop: reached event settling threshold, giving up.");
			break;
		}

		attempts += 1;
	} while (cnt != 0);

	// Swap in the end of the execution, else we might skip changes that happen
	// in the main loop for one frame - which is bad btw.
	this->queue.swap_changesets();
	log::log(DBG << "Loop: t=" << max_time << " was reached! ========");
}


int Loop::execute_events(const curve::time_t &time_until,
                         const std::shared_ptr<State> &state) {

	log::log(DBG << "Loop: Pending events in the queue (# = "
	         << this->queue.get_event_queue().size() << "):");

	{
		size_t i = 0;
		for (const auto &e : this->queue.get_event_queue().get_sorted_events()) {
			log::log(DBG << "  event "
			         << i << ": t=" << e->get_time() << ": " << e->get_eventclass()->id());
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

		auto target = event->get_target().lock();

		if (target) {
			log::log(DBG << "Loop: invoking event \"" << event->get_eventclass()->id()
			         << "\" on target \"" << target->id()
			         << "\" for time t=" << event->get_time());

			this->active_event = event;

			// apply the event effects
			event->get_eventclass()->invoke(
				*this, target, state,
				event->get_time(), event->get_params()
			);

			this->active_event = nullptr;
			cnt += 1;

			// if the event is REPEAT, readd the event.
			if (event->get_eventclass()->type == EventClass::trigger_type::REPEAT) {
				curve::time_t new_time = event->get_eventclass()->predict_invoke_time(
					target, state, event->get_time()
				);

				if (new_time != std::numeric_limits<curve::time_t>::min()) {
					event->set_time(new_time);

					log::log(DBG << "Loop: repeating event \"" << event->get_eventclass()->id()
					         << "\" on target \"" << target->id()
					         << "\" will be reenqueued for time t=" << event->get_time());

					this->queue.reenqueue(event);
				}
			}
		}
		else {
			// The element was already removed from the queue, so we can safely
			// kill it by ignoring it.
		}
	}
	return cnt;
}


void Loop::update_changes(const std::shared_ptr<State> &state) {

	log::log(DBG << "Loop: " << this->queue.get_changes().size()
	         << " target changes have to be processed");

	size_t i = 0;

	// reevaluate depending events beecause of the change
	for (const auto &change : this->queue.get_changes()) {
		auto evnt = change.evnt.lock();
		if (evnt) {
			log::log(DBG << "  change " << i++ << ": " << evnt->get_eventclass()->id());
			switch(evnt->get_eventclass()->type) {
			case EventClass::trigger_type::ONCE:
			case EventClass::trigger_type::DEPENDENCY: {
				auto target = evnt->get_target().lock();
				// TODO what happens when the target is degraded?

				if (target) {
					curve::time_t new_time = evnt->get_eventclass()
					                             ->predict_invoke_time(target, state, change.time);

					if (new_time != std::numeric_limits<curve::time_t>::min()) {
						log::log(DBG << "Loop: due to a change, rescheduling event of '"
						         << evnt->get_eventclass()->id()
						         << "' on target '" << target->id()
						         << "' at time t=" << change.time
						         << " to NEW TIME t=" << new_time);

						evnt->set_time(new_time);

						this->queue.enqueue_change(evnt);
					}
					else {
						log::log(DBG << "Loop: due to a change, canceled execution of '"
						         << evnt->get_eventclass()->id()
						         << "' on target '" << target->id()
						         << "' at time t=" << change.time);

						this->queue.remove(evnt);
					}
				}
			} break;

			case EventClass::trigger_type::TRIGGER:
			case EventClass::trigger_type::DEPENDENCY_IMMEDIATELY:
				evnt->set_time(change.time);
				this->queue.enqueue_change(evnt);
				break;

			case EventClass::trigger_type::REPEAT:
				break;
			}
		}
	}

	this->queue.clear_changes();
}

} // namespace openage::event
