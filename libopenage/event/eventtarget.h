// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#pragma once

#include "../curve/curve.h"

#include <functional>
#include <list>
#include <memory>

namespace openage::event {

class Event;
class Loop;
class EventClass;

/**
 * Every Object in the gameworld that wants to be targeted by events or as
 * dependency for events, has to implement this class.
 */
class EventTarget {
public:
	/** Give a unique event system identifier for the entity */
	virtual size_t id() const = 0;

	using single_change_notifier = std::function<void(const curve::time_t &)>;

protected:
	/**
	 * For children to be able to initialize us.
	 *
	 * The notifier is used by hierarchical structures to be able to traverse a
	 * change up in the tree, this is necessary to make containers with event
	 * targets inside and listen to any changes on the full.
	 */
	EventTarget(const std::shared_ptr<Loop> &loop,
	            single_change_notifier parent_notifier=nullptr)
		:
		loop{loop},
		parent_notifier{parent_notifier} {}
public:
	virtual ~EventTarget() = default;

	/**
	 * Add a dependent class, that should be notified when dependency is called
	 */
	void add_dependent(const std::weak_ptr<Event> &event);

	/**
	 * For debugging: print the dependent eventclass ids as log messages.
	 */
	void show_dependents() const;

protected:
	/**
	 * Call this whenever some data in the target changes.
	 * This triggers the reevaluation of dependent events.
	 */
	void changes(const curve::time_t &change_time);

	/**
	 * Call this when a keyframe in the underlying container was passed by in time.
	 */
	void trigger(const curve::time_t &last_valid_time);

private:
	/** Event loop this target is registered to */
	std::shared_ptr<Loop> loop;

	/** List of events that depend on this target */
	std::list<std::weak_ptr<Event>> dependents;

	single_change_notifier parent_notifier;
};

} // openage::event
