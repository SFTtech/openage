// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#pragma once

#include "../curve/curve.h"


#include <any>
#include <memory>
#include <string>
#include <unordered_map>


namespace openage::event {

class Event;
class Loop;
class EventTarget;
class State;


/**
 * A eventclass has to be implemented for every type of event that exists.
 * It determines what the event means and how it is handled.
 */
class EventClass {
public:
	/**
	 * Available types for the event class:
	 * These decide when an event of this event class will be executed.
	 */
	enum class trigger_type {
		/**
		 * Such events are emitted when a modification is done on a target.
		 * The execution time is calculated from the modification time and custom code.
		 */
		DEPENDENCY,

		/**
		 * Like DEPENDENCY, but does not use a recalculated time,
		 * instead uses the change-time only.
		 * Behaves exactly like DEPENDENCY, if the DEPENDENCY-event is in the same
		 * execution frame.
		 */
		DEPENDENCY_IMMEDIATELY,

		/**
		 * Will be executed when the target trigger() function is called.
		 */
		TRIGGER,

		/**
		 * Will be triggered unconditionally at the set time, "at" is the
		 * time that was set as return of predict_invoke_time. This event will
		 * be issued again until predict_invoke_time returns min(). To execute
		 * Something only once (i.E. triggered somewhere from the logic and
		 * not based on time, use ONCE
		 */
		REPEAT,

		/**
		 * Will be triggered only once, but until it is triggered the time,
		 * when this should happen can be recalculated again and again using
		 * the predict_invoke_time method.
		 */
		ONCE,
	};

	/**
	 * Storage for parameters for an event class.
	 */
	class param_map {
	public:
		using map_t = std::unordered_map<std::string, std::any>;

		param_map() {}
		param_map(std::initializer_list<map_t::value_type> l) : map(l) {}
		param_map(const map_t &map) : map{std::move(map)} {}

		/**
		 * Returns the value, if it exists and is the right type.
		 * defaultval if not.
		 */
		template<typename T>
		T get(const std::string &key, const T &defaultval=T()) const {
			auto it = this->map.find(key);
			if (it != this->map.end() && this->check_type<T>(it)) {
				return std::any_cast<T>(it->second);
			} else {
				return defaultval;
			}
		}

		/**
		 * Check if the map contains the given key.
		 */
		bool contains(const std::string &key) const {
			return map.find(key) != map.end();
		}

		/**
		 * Check if the type of a map entry is correct.
		 */
		template<typename Type>
		bool check_type(const std::string &key) const {
			auto it = map.find(key);
			if (it != map.end()) {
				return this->check_type<Type>(it);
			}
			return false;
		}

	private:
		template<typename T>
		bool check_type(const map_t::const_iterator &it) const {
			return it->second.type() == typeid(T);
		}

		/**
		 * Data storage.
		 */
		const map_t map;
	};

	/**
	 * Constructor to be constructed with the unique identifier
	 */
	EventClass(const std::string &name, const trigger_type &type);

	virtual ~EventClass() = default;

	/**
	 * The event type this event class represents.
	 */
	const trigger_type type;

	/**
	 * Return a unique identifier.
	 */
	const std::string &id();

	/**
	 * Called for each event that is created for this EventClass.
	 * The job of the setup function is to add all dependencies with other event
	 * targets found in state.
	 */
	virtual void setup_event(const std::shared_ptr<Event> &event,
	                         const std::shared_ptr<State> &state) = 0;

	/**
	 * This method implements the effects of the event.
	 * It will be called at the time that was determined by `predict_invoke_time`.
	 *
	 * Called from the Loop.
	 */
	virtual void invoke(Loop &loop,
	                    const std::shared_ptr<EventTarget> &target,
	                    const std::shared_ptr<State> &state,
	                    const curve::time_t &time,
	                    const param_map &params) = 0;

	/**
	 * Is called to calculate the execution time for an event of this eventclass.
	 * This is called whenever one of the set up dependencies was changed,
	 * or when a REPEAT event was executed.
	 *
	 * @param target: the target the event was created for
	 * @param state: the state this shall work on
	 * @param at: the time when the change happened, from there on it shall be
	 *            calculated onwards
	 *
	 * If the event is obsolete, return <time_t>::min().
	 *
	 * If the time is lower than the previous time,
	 * then dependencies may not be resolved perfectly anymore
	 * (if other events have already been calculated before that).
	 */
	virtual curve::time_t predict_invoke_time(const std::shared_ptr<EventTarget> &target,
	                                          const std::shared_ptr<State> &state,
	                                          const curve::time_t &at) = 0;

private:
	/**
	 * String identifier for this event class.
	 */
	std::string _id;
};


// helper classes

class DependencyEventClass : public EventClass {
public:
	DependencyEventClass(const std::string &name);
};

class DependencyImmediatelyEventClass : public EventClass {
public:
	DependencyImmediatelyEventClass(const std::string &name);
};

class TriggerEventClass : public EventClass {
public:
	TriggerEventClass(const std::string &name);
};

class RepeatEventClass : public EventClass {
public:
	RepeatEventClass(const std::string &name);
};

class OnceEventClass : public EventClass {
public:
	OnceEventClass(const std::string &name);
};


} // openage::event
