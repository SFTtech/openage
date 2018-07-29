// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#include "eventstore.h"

#include <algorithm>

#include "event.h"

#include "../util/compiler.h"
#include "../error/error.h"


namespace openage::event {

void EventStore::push(const std::shared_ptr<Event> &event) {
	if (unlikely(event == nullptr)) {
		throw Error{ERR << "inserting nullptr event to queue"};
	}

	heap_t::element_t order = this->heap.push(event);
	this->events.emplace(event, order);

	ENSURE(this->heap.size() == this->events.size(),
	       "heap and event set are inconsistent");
}


std::shared_ptr<Event> EventStore::pop() {
	ENSURE(this->heap.size() == this->events.size(),
	       "heap and event set are inconsistent 0");

	size_t heap_s = this->heap.size();
	size_t evnt_s = this->events.size();

	std::shared_ptr<Event> event = this->heap.pop();
	this->events.erase(event);

	if (this->heap.size() != this->events.size()) {
		throw Error{ERR << "inconsistent: prev_heap=" << heap_s
		                << " prev_map=" << evnt_s};
	}
		//ENSURE(this->heap.size() == this->events.size(),
		//   "heap and event set are inconsistent 1");

	return event;
}


const std::shared_ptr<Event> &EventStore::top() {
	return this->heap.top();
}


bool EventStore::erase(const std::shared_ptr<Event> &event) {
	bool erased = false;
	auto it = this->events.find(event);
	if (it != std::end(this->events)) {
		this->heap.unlink_node(it->second);
		this->events.erase(it);
		erased = true;
	}

	return erased;
}


void EventStore::update(const std::shared_ptr<Event> &event) {
	auto it = this->events.find(event);
	if (unlikely(it != std::end(this->events))) {
		this->heap.update(it->second);
	}
	else {
		throw Error{ERR << "event to update not found in store"};
	}
}


bool EventStore::contains(const std::shared_ptr<Event> &event) const {
	return (this->events.find(event) != std::end(this->events));
}


void EventStore::clear() {
	this->heap.clear();
	this->events.clear();
}


size_t EventStore::size() const {
	return this->events.size();
}


bool EventStore::empty() const {
	return this->events.empty();
}


std::vector<std::shared_ptr<Event>> EventStore::get_sorted_events() const {
	std::vector<std::shared_ptr<Event>> ret;

	ret.reserve(this->events.size());

	std::transform(
		std::begin(this->events),
		std::end(this->events),
		std::back_inserter(ret),
		[](const auto &elem) {
			return elem.first;
		}
	);

	std::sort(
		std::begin(ret),
		std::end(ret),
		[](const auto &a, const auto &b) {
			return *a < *b;
		}
	);

	return ret;
}

} // openage::event
