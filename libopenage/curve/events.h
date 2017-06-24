// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "curve.h"
#include "queue.h"

#include <functional>

namespace openage {
namespace curve {

class EventQueue {

public:
	typedef std::function<void(EventQueue *, const curve_time_t &)> callback;
	typedef std::function<bool(const curve_time_t &)> condition;

	typedef std::function<void(EventQueue *)> callback_void;
	typedef std::function<bool()> condition_void;

	class Event {
		friend class EventQueue;
	private:
		Event(const curve_time_t &time, const std::string &name, const callback &event, const condition &/*cond*/, const int event_id) :
			event_id{event_id},
			time{time},
			name(name),
			event{event}
			//precond(precond)
			{}
		int event_id;
	public:
		Event(const Event &rhs) :
			event_id(rhs.event_id), time(rhs.time), name(rhs.name), event(rhs.event) {}

		curve_time_t time;
		std::string name;
		callback event;
//		condition precond;
		bool precond(const curve_time_t &) const { return true; };
	};

	class Handle {
		friend class EventQueue;
	private:
		Handle(const int event_id) :
			event_id(event_id) {}

		const int event_id;
	};

	EventQueue(const curve_time_t &backlog_time = 1000) :
		cleared{false},
		backlog_time(backlog_time) {}

	Handle addcallback (const curve_time_t &at, const std::string &name, const condition &, const callback &);
	Handle addcallback (const curve_time_t &at, const std::string &name, const condition_void &, const callback_void &);

	Handle addcallback (const curve_time_t &at, const std::string &name, const callback &);
	Handle addcallback (const curve_time_t &at, const std::string &name, const callback_void &);

	Handle addcallback (const Event &);

	void execute_until(const curve_time_t &t);

	void clear();

	// Clear the queue from this time on.
	void clear(const curve_time_t &);

	size_t size() { return queue.size(); }

	void print();
private:
	std::deque<Event> queue;
	std::deque<Event> past;
	bool cleared;

	int last_event_id = 0;

	const curve_time_t backlog_time;
	static bool _true(const curve_time_t &);
};

}} // namespace openage::curve
