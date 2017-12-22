// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "eventmanager.h"

#include "event.h"
#include "eventclass.h"
#include "eventqueue.h"
#include "eventtarget.h"

#include "../../error/error.h"
#include "../../log/log.h"

namespace openage {
namespace curve {

void EventManager::changed(const std::shared_ptr<Event> &event, const curve_time_t &changed_at) {
	log::log(DBG << "Change: " << event->eventclass()->id() << " at " << changed_at);
	queue.add_change(event, changed_at);
}


void EventManager::add_class(const std::shared_ptr<EventClass> &cls) {
	classstore.insert(std::make_pair(cls->id(), cls));
}


std::weak_ptr<Event> EventManager::on(const std::string &name,
                                      const std::shared_ptr<EventTarget> &target,
                                      const std::shared_ptr<State> &state,
                                      const curve_time_t &reference_time,
                                      const EventClass::param_map &params) {
	auto it = classstore.find(name);
	if (it == classstore.end()) {
		// TODO FAIL
		throw Error(MSG(err) << "Trying to subscribe to eventclass " << name << ", which does not exist.");
	}
	return queue.add(target, it->second, state, reference_time, params);
}


std::weak_ptr<Event> EventManager::on(const std::shared_ptr<EventClass> &eventclass,
                                      const std::shared_ptr<EventTarget> &target,
                                      const std::shared_ptr<State> &state,
                                      const curve_time_t &reference_time,
                                      const EventClass::param_map &params) {
	auto it = classstore.find(eventclass->id());
	if (it == classstore.end()) {
		auto res = classstore.insert(std::make_pair(eventclass->id(), eventclass));
		if (res.second) {
			it = res.first;
		} else {
			return std::weak_ptr<Event>();
		}
	}

	return queue.add(target, it->second, state, reference_time, params);
}


void EventManager::execute_until(const curve_time_t &time,
                                 std::shared_ptr<::openage::State> &state) {
	int cnt = 0;
	do {
		update_changes(state);
		cnt = execute_events(time, state);
		log::log(DBG << "Executed " << cnt << " events");
	} while (cnt != 0);
	// Swap in the end of the execution, else we might skip changes that happen
	// in the main loop for one frame - which is bad btw.
	std::swap(queue.changes, queue.future_changes);
	log::log(DBG << "Cycle done");
}


int EventManager::execute_events(const curve_time_t &time_until,
                                 const std::shared_ptr<State> &state) {
	log::log(DBG << "Pending Events: ");
	for (const auto &e : queue.queue) {
		log::log(DBG << "\tEVENT: T:" << e->time() << ": " << e->eventclass()->id());
	}
	auto it = queue.queue.begin();
	int cnt = 0;
	while ((it = queue.queue.begin()) != queue.queue.end() && (*it)->time() < time_until) {
		auto tmp = *it;
		queue.queue.pop_front();
		auto target = tmp->target().lock();
		if (target) {
			this->active_event = tmp;

			log::log(DBG << "Calling Event \"" << tmp->eventclass()->id()
			         << "\" on Element \"" << target->id()
			         << "\" at time " << tmp->time());

			tmp->eventclass()->call(this, target, state, tmp->time(), tmp->params);
			active_event = nullptr;
			cnt ++;

			if (tmp->eventclass()->type == EventClass::Type::ON_EXECUTE) {
				auto new_time = tmp->eventclass()->recalculate_time(target, state, tmp->time());
				if (new_time != -std::numeric_limits<curve_time_t>::max()) {
					tmp->time(new_time);
					queue.readd(tmp);
				}
			}
		} else {
			// The element was already removed from the queue, so we can safely
			// kill it by ignoring it.
		}
	}
	return cnt;
}


void EventManager::update_changes(const std::shared_ptr<State> &state) {
	// The second magic happens here
	log::log(DBG << "Updating " << queue.changes->size() << " changes");
	for (const auto &change : *queue.changes) {
		auto evnt = change.evnt.lock();
		if (evnt) {
			switch(evnt->eventclass()->type) {
			case EventClass::Type::ONCE:
			case EventClass::Type::ON_CHANGE: {
				auto target = evnt->target().lock();
				// TODO what happens when the target is degraded?
				if (target) {
					curve_time_t new_time = evnt->eventclass()
					                        ->recalculate_time(target, state, change.time);
					log::log(DBG << "Recalculating Event " << evnt->eventclass()->id() << " on Element " << target->id() << " at time " << change.time << ", new time: " << new_time);
					if (new_time != -std::numeric_limits<curve_time_t>::max()) {
						evnt->time(new_time);
						queue.update(evnt);
					}
				}
			} break;
			case EventClass::Type::ON_KEYFRAME:
			case EventClass::Type::ON_CHANGE_IMMEDIATELY:
				evnt->time(change.time);
				queue.update(evnt);
				break;
			case EventClass::Type::ON_EXECUTE:
				break;
			}
		}
	}
	queue.changes->clear();
}

}} // namespace openage::curve
