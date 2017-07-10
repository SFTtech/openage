// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp.string cimport string
#include "curve.h"
#include "queue.h"

#include <functional>
#include <vector>

namespace openage {
namespace curve {

class Trigger;

/**
 * Event container that can execute and manage the stuff.
 *
 * pxd:
 * cppclass EventQueue:
 *     ctypedef string Eventclass
 *
 * cppclass EventQueue__Handle "openage::curve::EventQueue::Handle":
 *      pass
 *
 */
class EventQueue {
public:
	/** Callback with eventqueue and time as arguments */
	typedef std::function<void(EventQueue *, const curve_time_t &)> callback;

	/** Callback with only the eventqueue as argument */
	typedef std::function<void(EventQueue *)> callback_void;

	typedef std::function<curve_time_t(const curve_time_t &)> time_predictor;

	/** Class of events - this identifies a type of event */
	typedef std::string Eventclass;

	/**
	 * One single event that will be executed. This can be rescheduled, but the
	 * event id will never change.
	 */
	class Event {
		friend class EventQueue;
	private:
		/** An event can only be constructed in the event queue */
		Event(const time_predictor &predictor,
		      const curve_time_t &initial_now,
		      const Eventclass &eventclass,
		      const callback &event,
		      const int event_id,
		      const std::vector<Trigger *> &dependents,
		      bool repeat = false) :
			event_id{event_id},
			dependents{dependents},
			predictor{predictor},
			time{predictor(initial_now)},
			eventclass(eventclass),
			event{event},
			repeat{repeat} {}

		/** The Event id is only relevant within the eventqueue */
		int event_id;

		std::vector<Trigger *> dependents;

		time_predictor predictor;
	public:
		Event() = default;

		/** Copy c'tor */
		//Event(const Event &rhs) = default;

		/** when will this be executed */
		curve_time_t time;
		/** class of the event, used for identification */
		Eventclass eventclass;
		/** The event itself */
		callback event;

		bool repeat;

	};


	/**
	 * Handle to be able to communicate with the event that has been stored.
	 * uses Event::event_id for identification.
	 *
	 */
	class Handle {
		friend class EventQueue;
	public:
		Handle() :
			Handle(0) {};
	private:
		/** Can only be constructed from the EventQueue scope */
		explicit Handle(const int event_id) :
			event_id(event_id) {}

		/** Identificator */
		int event_id;
	};

	/** Default c'ctor */
	EventQueue(const curve_time_t &backlog_time = 1000) :
		cleared{false},
		backlog_time(backlog_time) {}

	/**
	 * Add a callback - with an event class that takes queue and time as
	 * arguments
	 */
	Handle addcallback (
		const Eventclass &eventclass,
		const curve_time_t &initial_now,
		const time_predictor &at,
		const callback &event,
		const std::vector<Trigger *> dependents);

	/**
	 * Add a callback - with an event class that takes only queue as arguments
	 */
	Handle addcallback(
		const Eventclass &eventclass,
		const curve_time_t &initial_now,
		const time_predictor &at,
		const callback_void &event,
		const std::vector<Trigger *> dependents);

	Handle schedule(
		const Eventclass &eventclass,
		const curve_time_t &initial_now,
		const time_predictor &at,
		const callback &event,
		const std::vector<Trigger *> dependents);

	/**
	 * Take an event and reschedule the time, if it has not yet been executed
	 */
	void reschedule(const Handle &, const time_predictor &timer, const curve_time_t &at);

	/**
	 * Take an event and reschedule the time, if it has not yet been executed
	 */
	void reschedule(const Handle &, const curve_time_t &at);

	/**
	 * Reschedule the currently running event to the time given
	 */
	void reschedule(const time_predictor &timer, const curve_time_t &at);

	/**
	 * Reschedule the Event and reevaluate the timer
	 */
	void reschedule(const curve_time_t &at);

	/**
	 * Iteratively take the events and execute them one by one. The Queue may
	 * be changed during execution.
	 */
	void execute_until(const curve_time_t &t);

	/**
	 * Remove an event from execution, if it has not been executed yet.
	 */
	void remove(const Handle &);

	/**
	 * Remove all events from the queue
	 */
	void clear();

	/**
	 * Clear the queue from this time on.
	 */
	void clear(const curve_time_t &);

	/**
	 * Count the number of events that might be executed. This is only a internal
	 * identification number.
	 */
	size_t size() { return queue.size(); }

	/**
	 * Number printer
	 */
	void print();
private:
	Handle addcallback(const Event &event);

	std::deque<Event>::iterator resolve_handle(const Handle &handle);
	std::deque<Event> queue;
	std::deque<Event> past;
	Event *active_event;

	bool cleared;
	int last_event_id = 0;
	const curve_time_t backlog_time;
};

}} // namespace openage::curve
