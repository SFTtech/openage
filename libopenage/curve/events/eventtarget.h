// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "../curve.h"

#include <functional>
#include <list>
#include <memory>

namespace openage {
namespace curve {

class Event;
class EventManager;
class EventClass;

/**
 * Every Object in the gameworld that wants to be targeted by events or as
 * dependency for events, has to implement this class.
 */
class EventTarget {
	friend EventClass;
public:
	/** Give a unique identifier for the unit */
	virtual size_t id() const = 0;

	using single_change_notifier = std::function<void(const curve_time_t &)>;
protected:
	/**
	 * For children to be able to initialize us.
	 *
	 * The notifier is used by hierarchical structures to be able to traverse a
	 * change up in the tree, this is necessary to make containers with event
	 * targets inside and listen to any changes on the full.
	 */
	EventTarget(EventManager *manager, single_change_notifier parent_notifier = nullptr) :
		manager{manager},
		parent_notifier{parent_notifier} {}

	/** Whenever some data in the Target changed */
	void on_change(const curve_time_t &);

	/** when a keyframe in the underlying container was passed. */
	void on_pass_keyframe(const curve_time_t &last_valid_time);

	/** add a dependent class, that should be notified when on_change is called */
	void add_dependent(const std::weak_ptr<Event> &event);

private:
	EventManager *manager;
	std::list<std::weak_ptr<Event>> dependents;
	single_change_notifier parent_notifier;
};

}} // namespace openage::curve
