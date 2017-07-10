// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "trigger.h"

#include <sstream>

namespace openage {
namespace curve {

Trigger::Trigger(class TriggerFactory *factory) :
	name(factory->generate_uuid(this)),
	factory{factory} {
}


Trigger::~Trigger() {
	clearevents();
	factory->remove(this);
}


void Trigger::on_change_future(const EventQueue::Eventclass &eventclass,
                               const Trigger::change_event &event) {
	EventTrigger trg{event, this->name + eventclass, 0};
	change_future_queue.insert(insert_pos(trg, change_future_queue), trg);
}


void Trigger::on_pass_keyframe(const EventQueue::Eventclass &eventclass,
                               const Trigger::change_event &event) {
	EventTrigger trg{event, this->name + eventclass, 0};
	change_pass_keyframe_queue.insert(insert_pos(trg, change_pass_keyframe_queue), trg);
}


void Trigger::on_pre_horizon(const curve_time_t &time,
                             const EventQueue::Eventclass &eventclass,
                             const Trigger::change_event &event) {
	EventTrigger trg{event, this->name + eventclass, time};
	factory->enqueue(this, trg, time);
}


void Trigger::on_change(const curve_time_t &time,
                        const EventQueue::Eventclass &eventclass,
                        const Trigger::change_event &event) {
	EventTriggerChange trg{event, this->name + eventclass, time};
	change_queue.insert(insert_pos(trg, change_queue), trg);
}


void Trigger::clearevents() {
	factory->remove(this);
	change_future_queue.clear();
	change_pass_keyframe_queue.clear();
	change_queue.clear();
}


void Trigger::data_changed(const curve_time_t &now,
                           const curve_time_t &/*changed_at*/) {
	// on_change_future will always happen when something changes
	for (auto &e : change_future_queue) {
		// TODO Determine if "now" or "changed_at" is more convenient
		handles.push_back(EventTriggerHandle(e, factory->enqueue(this, e, now)));
	}

	for (auto &e : change_queue) {
		if (e.is_inserted) {
			factory->reschedule(e.handle, now);
		} else {
			EventQueue::Handle h = factory->enqueue(this, e, now);
			e.handle = h;
			e.is_inserted = true;
		}
	}
}


void Trigger::passed_keyframe(const curve_time_t &time) {
	for (auto &e : change_pass_keyframe_queue) {
		handles.push_back(EventTriggerHandle(e, factory->enqueue(this, e, time)));
	}
}


EventQueue::Handle TriggerFactory::enqueue(Trigger *trigger,
                                           const Trigger::EventTrigger &trg,
                                           const curve_time_t &at) {
	auto t = trg.time;
	return queue->addcallback(
		trg.eventclass,
		at,
		[t](const curve_time_t &) {
			return t;
		},
		trg.event,
		{trigger});
}


EventQueue::Handle TriggerFactory::enqueue(Trigger *trigger,
                                           const Trigger::EventTrigger &trg,
                                           const EventQueue::time_predictor &timer,
                                           const curve_time_t &at) {
	return queue->addcallback(
		trg.eventclass,
		at,
		timer,
		trg.event,
		{trigger}
	);
}


void TriggerFactory::remove(Trigger *t) {
	for (const auto &handle : t->handles) {
		queue->remove(handle.next_handle);
	}
	t->handles.clear();
}


void TriggerFactory::reschedule(const EventQueue::Handle &handle,
                                const EventQueue::time_predictor &new_timer,
                                const curve_time_t &at) {
	queue->reschedule(handle, new_timer, at);
}


void TriggerFactory::reschedule(const EventQueue::Handle &handle,
                                const curve_time_t &at) {
	queue->reschedule(handle, at);
}


std::string TriggerFactory::generate_uuid(void const *ptr) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%p", ptr);
	return std::string(buf);
}


TriggerIntermediateMaster::TriggerIntermediateMaster(TriggerFactory *superfactory) :
	Trigger(this), TriggerFactory(superfactory->queue) { }


}} // namespace openage::curve
