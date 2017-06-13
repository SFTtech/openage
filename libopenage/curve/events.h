// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "curve.h"
#include "queue.h"

#include <functional>

namespace openage {
namespace curve {

class EventQueue {

public:
	typedef std::function<void(const curve_time_t &)> callback;
	typedef std::function<bool(const curve_time_t &)> condition;

	typedef std::function<void(void)> callback_void;
	typedef std::function<bool(void)> condition_void;

	class Event {
	public:
		callback event;
		condition precond;
		curve_time_t time;
	};

	void addcallback (const curve_time_t &at, const condition &, const callback &);
	void addcallback (const curve_time_t &at, const condition_void &, const callback_void &);

	void addcallback (const curve_time_t &at, const callback &);
	void addcallback (const curve_time_t &at, const callback_void &);

	void addcallback (const Event &);

	void trigger(const curve_time_t &from, const curve_time_t &to);

private:
	Queue<Event> queue;

	static bool _true(const curve_time_t &);
};

}} // namespace openage::curve
