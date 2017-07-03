// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "trigger.h"

namespace openage {
namespace curve {

Trigger::Trigger(class TriggerFactory *factory) :
	factory{factory} {
}


Trigger::~Trigger() {
	clearevents();
	factory->remove(this);
}


void Trigger::on_change_future(const EventQueue::Eventclass &eventclass,
                               const Trigger::change_event &event) {
	EventTrigger trg{event, eventclass, 0};
	change_future_queue.insert(insert_pos(trg, change_future_queue), trg);
}


void Trigger::on_pass_keyframe(const EventQueue::Eventclass &eventclass,
                               const Trigger::change_event &event) {
	EventTrigger trg{event, eventclass, 0};
	change_pass_keyframe_queue.insert(insert_pos(trg, change_pass_keyframe_queue), trg);
}


void Trigger::on_pre_horizon(const curve_time_t &time,
                             const EventQueue::Eventclass &eventclass,
                             const Trigger::change_event &event) {
	EventTrigger trg{event, eventclass, time};
	factory->enqueue(this, trg);
}


void Trigger::on_change(const curve_time_t &time,
                        const EventQueue::Eventclass &eventclass,
                        const Trigger::change_event &event) {
	EventTriggerChange trg{event, eventclass, time};
	change_queue.insert(insert_pos(trg, change_queue), trg);
}


void Trigger::clearevents() {
	factory->remove(this);
	change_future_queue.clear();
	change_pass_keyframe_queue.clear();
	change_queue.clear();

}


void Trigger::data_changed(const curve_time_t &now,
                           const curve_time_t &changed_at) {
	// on_change_future will always happen when something changes
	for (auto &e : change_future_queue) {
		handles.push_back(EventTriggerHandle(e, factory->enqueue(this, e, now)));
	}

	for (auto &e : change_queue) {
		if (e.is_inserted) {
			factory->reschedule(e.handle, changed_at);
		} else {
			EventQueue::Handle h = factory->enqueue(this, e);
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


EventQueue::Handle TriggerFactory::enqueue(Trigger */*trigger*/, const Trigger::EventTrigger &trg) {
	return queue->addcallback(trg.time, trg.eventclass, trg.event);
}


EventQueue::Handle TriggerFactory::enqueue(Trigger */*trigger*/,
                                           const Trigger::EventTrigger &trg,
                                           const curve_time_t &time) {
	return queue->addcallback(time, trg.eventclass, trg.event);
}


void TriggerFactory::remove(Trigger *t) {
	for (const auto &handle : t->handles) {
		queue->remove(handle.next_handle);
	}
	t->handles.clear();
}


void TriggerFactory::reschedule(const EventQueue::Handle &handle,
                                const curve_time_t &new_time) {
	queue->reschedule(handle, new_time);
}

}} // namespace openage::curve
