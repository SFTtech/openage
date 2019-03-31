// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#pragma once

#include "../curve/curve.h"

#include <functional>
#include <list>
#include <memory>

namespace openage::event {

class Event;
class Loop;
class EventHandler;

/**
 * Every Object in the gameworld that wants to be targeted by events or as
 * dependency for events, has to implement this class.
 */
class EventEntity {
public:
	/** Give a unique event system identifier for the entity */
	virtual size_t id() const = 0;

	/** Give a human-readable identifier for this target */
	virtual std::string idstr() const = 0;

	using single_change_notifier = std::function<void(const curve::time_t &)>;

protected:
	/**
	 * For children to be able to initialize us.
	 *
	 * The notifier is used by hierarchical structures to be able to propagate a
	 * change up in the tree, this is necessary to make containers with event
	 * targets inside and listen to any changes on the full.
	 */
	EventEntity(const std::shared_ptr<Loop> &loop,
	            single_change_notifier parent_notifier=nullptr)
		:
		loop{loop},
		parent_notifier{parent_notifier} {}
public:
	virtual ~EventEntity() = default;

	/**
	 * Add a dependent event that is notified whenever this entity changes.
	 * Does not support TRIGGER and REPEAT event types.
	 */
	void add_dependent(const std::shared_ptr<Event> &event);

	/**
	 * For debugging: print the dependent eventhandler ids as log messages.
	 */
	void show_dependents() const;

protected:
	/**
	 * Call this whenever some data in the target changes.
	 * This triggers the reevaluation of dependent events.
	 */
	void changes(const curve::time_t &change_time);

	/**
	 * Call this when depending TriggerEventHandleres should be invoked.
	 */
	void trigger(const curve::time_t &invoke_time);

private:
	/** Event loop this target is registered to */
	std::shared_ptr<Loop> loop;

	/** List of events that depend on this target */
	std::list<std::weak_ptr<Event>> dependents;

	single_change_notifier parent_notifier;
};

} // openage::event
