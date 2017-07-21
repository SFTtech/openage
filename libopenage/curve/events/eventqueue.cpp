// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "eventqueue.h"

#include "event.h"
#include "eventclass.h"
#include "eventtarget.h"

#include "../../error/error.h"
#include "../../log/log.h"

namespace openage {
namespace curve {

std::weak_ptr<Event> EventQueue::add(const std::shared_ptr<EventTarget> &trgt,
                                     const std::shared_ptr<EventClass> &cls,
                                     const std::shared_ptr<State> &state,
                                     const curve_time_t &reference_time,
                                     const EventClass::param_map &params) {
	auto sp = std::make_shared<Event>(trgt, cls, params);

	cls->setup(sp, state);
	switch(cls->type) {
	case EventClass::Type::ON_CHANGE:
	case EventClass::Type::ON_EXECUTE:
	case EventClass::Type::ONCE:
		sp->time(sp->eventclass()->recalculate_time(trgt, state, reference_time));
		if (sp->time() == -std::numeric_limits<curve_time_t>::max()) {
			return std::weak_ptr<Event>();
		}
		break;
	case EventClass::Type::ON_CHANGE_IMMEDIATELY:
	case EventClass::Type::ON_KEYFRAME:
		sp->_time = reference_time;
		break;
	}
	select_queue(sp->eventclass()->type).emplace_back(sp);

	this->check_in(sp);

	return sp;
}


EventQueue::EventQueue() :
	changes(&changeset_A), future_changes(&changeset_B) {}


void EventQueue::check_in(const std::shared_ptr<Event> &evnt) {
	switch(evnt->eventclass()->type) {
	case EventClass::Type::ON_CHANGE:
	case EventClass::Type::ON_CHANGE_IMMEDIATELY:
		// TODO Set up these fellows
		break;
	case EventClass::Type::ON_KEYFRAME:
		// TODO set up this fellow
		break;
	case EventClass::Type::ON_EXECUTE:
		// Add this to the event queue, although it is already in the other queue
		queue.emplace_back(evnt);
		break;
	case EventClass::Type::ONCE:
		// Has already been added to the queue, so nothing to do here
		break;
	}
}


void EventQueue::add_change(const std::shared_ptr<Event> &event,
                            const curve_time_t &changed_at) {

	auto it = changes->find(OnChangeElement(event, changed_at));

	// Has the event been triggered in this round?
	if (event->last_triggered < changed_at) {
		// Is the change already in the queue?
		if (it != changes->end()) {
			// Is the new change dated _before_ the old one?
			if (it->time > changed_at) {
				// Save the element
				OnChangeElement e = *it;
				e.time = changed_at;
				// delete it from the container and readd it
				it = changes->erase(it);
				it = changes->insert(it, e);
				log::log(DBG << "adjusting time in change queue");
			} else {
				// this change is to be ignored
				log::log(DBG << "skipping change " << event->eventclass()->id()
				         << " at " << changed_at
				         << " because there was already a better one at " << it->time);
			}
		} else {
			// the change was not in the to be changed list
			changes->emplace(event, changed_at);
			log::log(DBG << "inserting change " << event->eventclass()->id() << " at " << changed_at);
		}
	} else {
		// the event has been triggered in this round already, so skip it this time
		future_changes->emplace(event, changed_at);
		log::log(DBG << "putting change into the future: " << event->eventclass()->id() << " at " << changed_at);
	}
	event->last_triggered = changed_at;
}


void EventQueue::remove(const std::shared_ptr<Event> &evnt) {
	auto &queue = select_queue(evnt->eventclass()->type);
	auto it = std::find(std::begin(queue), std::end(queue), evnt);

	if (it != queue.end()) {
		queue.erase(it);
	}
}

std::deque<std::shared_ptr<Event>> &EventQueue::select_queue(EventClass::Type type) {
	switch(type) {
	case EventClass::Type::ON_CHANGE:
		return on_change;
		break;
	case EventClass::Type::ON_CHANGE_IMMEDIATELY:
		return on_change_immediately;
		break;
	case EventClass::Type::ON_KEYFRAME:
		return on_keyframe;
		break;
	case EventClass::Type::ON_EXECUTE:
		return on_execute;
		break;
	default:
	case EventClass::Type::ONCE:
		return queue;
		break;
	}
}


void EventQueue::update(const std::shared_ptr<Event> &evnt) {
	decltype(queue)::iterator it = std::find(std::begin(queue), std::end(queue), evnt);
	if (it != queue.end()) { // we only want to update the pending queue
		int dir = 0;
		{
			auto nxt = std::next(it);
			if (nxt != std::end(queue) && (*nxt)->time() < (*it)->time()) {
				dir --;
			}
			auto prv = std::prev(it);
			if (it != std::begin(queue) && (*prv)->time() > (*it)->time()) {
				dir ++;
			}
			// dir is now one of these values:
			// 0: the element is perfectly placed
			// +: the elements time is too low for its position, move forward
			// -: the elements time is too high for its position, move backwards
		}
		if (dir == 0) {
			// do nothing
		} else if (dir > 0) {
			auto e = *it;
			it = queue.erase(it);
			while (it != queue.begin()
			      && evnt->time() < (*std::prev(it))->time()) {
				it--;
			}
			queue.insert(it, e);
		} else if (dir < 0) {
			auto e = *it;
			it = queue.erase(it);
			while (it != queue.end()
			      && evnt->time() > (*std::prev(it))->time()) {
				it++;
			}
			queue.insert(it, e);
		}
		log::log(DBG << "Readded Element: " << evnt->eventclass()->id() << " at " << evnt->time());
	} else {
		it = std::find_if(
			std::begin(queue),
			std::end(queue),
			[&evnt] (const std::shared_ptr<Event> &e) {
				return e->time() > evnt->time();
			});
		queue.insert(it, evnt);
		log::log(DBG << "Inserted Element: " << evnt->eventclass()->id() << " at " << evnt->time());
	}
}


void EventQueue::readd(const std::shared_ptr<Event> &evnt) {
	auto &container = select_queue(evnt->eventclass()->type);

	auto it = std::find_if(
		std::begin(container),
		std::end(container),
		[&evnt](const std::shared_ptr<Event> &element) {
			return element->time() < evnt->time();
		});
	container.insert(it, evnt);

	check_in(evnt);
}


EventQueue::OnChangeElement::OnChangeElement(const std::shared_ptr<Event> &evnt,
                                             const curve_time_t &time) :
	time{time},
	evnt{evnt},
	hash{evnt->hash()} {}


size_t EventQueue::OnChangeElement::Equal::operator()(const OnChangeElement& left,
                                                      const OnChangeElement& right) const {
	auto left_evnt = left.evnt.lock();
	auto right_evnt = right.evnt.lock();
	if (left_evnt && right_evnt) {
		if (left_evnt->eventclass()->id() == right_evnt->eventclass()->id()) {
			return true;
		}
	} else {
		return false;
	}

	auto left_trgt = left_evnt->target().lock();
	auto right_trgt = right_evnt->target().lock();

	return left_trgt && right_trgt &&
	left_trgt->id() == right_trgt->id();
}


}} // namespace openage::curve
