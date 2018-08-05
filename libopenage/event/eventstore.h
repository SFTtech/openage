// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#pragma once

#include "../datastructure/pairing_heap.h"
#include "../util/misc.h"

#include <memory>
#include <unordered_map>


namespace openage::event {

class Event;

/**
 * Sorted storage for events.
 * Implemented through a heap that automatically provides the newest event.
 */
class EventStore {
public:

	// TODO: don't store a double-sharedpointer.
	//       instead, use the event-sharedpointer directly.
	using heap_t = datastructure::PairingHeap<std::shared_ptr<Event>,
	                                          util::SharedPtrLess<Event>>;
	using elemmap_t = std::unordered_map<std::shared_ptr<Event>, heap_t::element_t>;

	void push(const std::shared_ptr<Event> &event);
	std::shared_ptr<Event> pop();
	const std::shared_ptr<Event> &top();
	bool erase(const std::shared_ptr<Event> &event);
	void update(const std::shared_ptr<Event> &event);
	bool contains(const std::shared_ptr<Event> &event) const;
	void clear();
	size_t size() const;
	bool empty() const;

	/**
	 * Helper function that should not be called 'in production'.
	 * It returns the events in the store sorted by time.
	 * Use the pop() method instead (but with that you can't iterate).
	 */
	std::vector<std::shared_ptr<Event>> get_sorted_events() const;

	heap_t heap;
	elemmap_t events;
};


} // openage::event
