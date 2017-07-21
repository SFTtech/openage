// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "../curve.h"

#include "eventqueue.h"

#include <functional>
#include <memory>
#include <map>
#include <list>

// The demo wants to display details
namespace openage {
class State;
namespace curvepong {
int demo();
}
}


namespace openage {
class State;
namespace curve {

class EventTarget;
class Event;

class EventFilter {
public:
	EventFilter(std::function<bool(const std::shared_ptr<EventTarget> &)> &filter) :
		_filter(filter) {}

	bool apply(const std::shared_ptr<EventTarget> target) {
		return _filter(target);
	}
private:
	std::function<bool(const std::shared_ptr<EventTarget>&)> _filter;
};

/**
 * The core class to manage event class and targets.
 */
class EventManager {
	friend int openage::curvepong::demo();
public:
	void add_class(const std::shared_ptr<EventClass> &cls);

	std::weak_ptr<Event> on(const std::string &name,
	                        const std::shared_ptr<EventTarget> &target,
	                        const std::shared_ptr<State> &state,
	                        const curve_time_t &reference_time,
                            const EventClass::param_map &params = EventClass::param_map({}));
	/**
	 * This will generate a new randonmly named eventclass for this specific element
	 */
	std::weak_ptr<Event> on(const std::shared_ptr<EventClass> &eventclass,
	                        const std::shared_ptr<EventTarget> &target,
	                        const std::shared_ptr<State> &state,
	                        const curve_time_t &reference_time,
                            const EventClass::param_map &params = EventClass::param_map({}));

	void onfilter(const std::shared_ptr<EventClass> &eventclass, const EventFilter &);

	template <class evntclass_t>
	void onfilter(const EventFilter &filter) {
		onfilter(std::make_shared<evntclass_t>(), filter);
	}

	void refilter(const std::shared_ptr<EventTarget> &);
	void refilter(const std::shared_ptr<Event> &);

	void register_object(const std::shared_ptr<EventTarget> &);

	/**
	 * Submit a change in a dependent value to an event. Insert it into the
	 * changes queue and evaluate it accordingly
	 */
	void changed(const std::shared_ptr<Event> &event,
	             const curve_time_t &changed_at);

	/**
	 * Execute all events that are registered until a certain point in time.
	 */
	void execute_until(const curve_time_t &time, std::shared_ptr<::openage::State> &state);

private:
	/**
	 * Execute the events
	 *
	 * \return number of events processed
	 */
	int execute_events(const curve_time_t &time,
	                   const std::shared_ptr<State> &state);

	/**
	 * Call all the time change functions. This is constant on the state!
	 */
	void update_changes(const std::shared_ptr<State> &state);

	/// Here we do the bookkeeping of registered classes.
	std::map<std::string, std::shared_ptr<EventClass>> classstore;

	/**
	 * Here we store all running filters that shall be applied whenever a new
	 * obejct is added to our objectstore
	 */
	std::list<EventFilter> filters;

	EventQueue queue;
	std::shared_ptr<Event> active_event;
};

}} // namespace openage::curve
