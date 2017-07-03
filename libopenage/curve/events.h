// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "curve.h"
#include "queue.h"

#include <functional>

namespace openage {
namespace curve {

/**
 * Event container that can execute and manage the stuff.
 */
class EventQueue {
public:
	/** Callback with eventqueue and time as arguments */
	typedef std::function<void(EventQueue *, const curve_time_t &)> callback;

	/** Callback with only the eventqueue as argument */
	typedef std::function<void(EventQueue *)> callback_void;

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
		Event(const curve_time_t &time,
		      const Eventclass &eventclass,
		      const callback &event,
		      const int event_id) :
			event_id{event_id},
			time{time},
			eventclass(eventclass),
			event{event} {}

		/** The Event id is only relevant within the eventqueue */
		int event_id;
	public:
		/** Copy c'tor */
		Event(const Event &rhs) :
			event_id(rhs.event_id),
			time(rhs.time),
			eventclass(rhs.eventclass),
			event(rhs.event) {}

		/** when will this be executed */
		curve_time_t time;
		/** class of the event, used for identification */
		Eventclass eventclass;
		/** The event itself */
		callback event;
	};


	/**
	 * Handle to be able to communicate with the event that has been stored.
	 * uses Event::event_id for identification.
	 */
	class Handle {
		friend class EventQueue;
	public:
		Handle() :
			Handle(0) {};

	private:
		/** Can only be constructed from the EventQueue scope */
		Handle(const int event_id) :
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
	Handle addcallback (const curve_time_t &at, const Eventclass &eventclass, const callback &);

	/**
	 * Add a callback - with an event class that takes only queue as arguments
	 */
	Handle addcallback (const curve_time_t &at, const Eventclass &eventclass, const callback_void &);

	/** Add a callback created from an event */
	Handle addcallback (const Event &);

	/**
	 * Take an event and reschedule the time, if it has not yet been executed
	 */
	void reschedule(const Handle &, const curve_time_t &time);

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
	std::deque<Event>::iterator resolve_handle(const Handle &handle);
	std::deque<Event> queue;
	std::deque<Event> past;

	bool cleared;
	int last_event_id = 0;
	const curve_time_t backlog_time;
};

}} // namespace openage::curve
