// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp.string cimport string
// pxd: from libopenage.curve.events cimport EventQueue
// pxd: from libopenage.curve.curve cimport curve_time_t
#include "events.h"

#include <vector>

namespace openage {
namespace curve {

/* Forward declaration of testing methods */
namespace tests {
void trigger();
}

class TriggerFactory;

/**
 * A Trigger is a entity, that where one can subscripe its change interface.
 * It will keep track of everything for change observation on top of event queue.
 *
 * pxd:
 *
 * cppclass Trigger:
 *      ctypedef void (*change_event)(EventQueue *, const curve_time_t &)
 *      void Trigger(TriggerFactory *) except +
 *      void on_change_future(const string &, const Trigger.change_event &) except +
 *      void on_pass_keyframe(const string &, const Trigger.change_event &) except +
 *      void on_pre_horizon(const curve_time_t &, const string &, const change_event &) except +
 *      void on_change(const curve_time_t &, const string &, const change_event &) except +
 *      void clearevents() except +
 */
class Trigger {
	friend class TriggerFactory;
	friend void tests::trigger(); // the test needs access to data_canged
public:
	typedef std::function<void(EventQueue *, const curve_time_t &)> change_event;

	const std::string name;
	Trigger(class TriggerFactory *factory);

	virtual ~Trigger();

	/**
	 * Will be triggered when anything changes - immediately.
	 */
	void on_change_future(const EventQueue::Eventclass &, const change_event & );

	/**
	 * Will be triggered whenever you pass a keyframe, that is not relevant for execution anymore
	 */
	void on_pass_keyframe(const EventQueue::Eventclass &, const change_event &);

	/**
	 * Will be triggred whenever execution passes this event frame
	 */
	void on_pre_horizon(const curve_time_t &time, const EventQueue::Eventclass &, const change_event &);

	/**
	 * When the queue has changed, this event will be triggered at the referenced
	 * time frame/
	 */
	void on_change(const curve_time_t &time, const EventQueue::Eventclass &, const change_event &);

	/**
	 * remove all events that are registered with the event queue at the moment
	 */
	void clearevents();
protected:

	/**
	 * This method is to be called whenever a keyframe changes. It will take care
	 * to enqueue the appropriate events into the main event queue.
	 */
	void data_changed(const curve_time_t &now, const curve_time_t &changed_at);

	/** TODO this method is still to do. */
	void passed_keyframe(const curve_time_t &);

private:
	TriggerFactory *factory;

	/**
	 * Contains all relevant information for triggering of events
	 */
	struct EventTrigger {
		EventTrigger(const change_event &event,
		             const EventQueue::Eventclass &eventclass,
		             const curve_time_t time) :
			event{event},
			eventclass{eventclass},
			time{time} {}
		/// The callback to invoke
		change_event event;

		/// The event group this is related
		EventQueue::Eventclass eventclass;

		/// The time this event is scheduled
		curve_time_t time;
	};

	/**
	 * Contains all relevant information that are contained in the pending queue
	 */
	struct EventTriggerHandle {
		EventTriggerHandle(const EventTrigger &trigger,
		                   const EventQueue::Handle &handle) :
			trigger{trigger},
			next_handle{handle} {}
		EventTrigger trigger;
		EventQueue::Handle next_handle;
	};

	/**
	 * Specific trigger for on_change events
	 **/
	class EventTriggerChange : public EventTrigger {
	public:
		using EventTrigger::EventTrigger;
		bool is_inserted = false;
		EventQueue::Handle handle;
	};

	/**
	 * Enqueued event handles - these events are registered with the referenced
	 * factory
	 */
	std::vector<EventTriggerHandle> handles;

	/**
	 * Storage of registered events, that are not only stored within the global
	 * event queue
	 */
	std::vector<EventTrigger> change_future_queue;
	std::vector<EventTrigger> change_pass_keyframe_queue;
//	std::vector<EventTrigger> pre_horizon_queue; // This is directly implemented in the global queue
	std::vector<EventTriggerChange> change_queue;

	/**
	 * Magic to identify the insertion position for a TriggerType.
	 */
	template<typename InsertQueue, typename TriggerType>
	typename InsertQueue::iterator insert_pos(const TriggerType &trg,
	                                          InsertQueue &queue) {
		auto it = queue.begin();
		while (it != queue.end() && it->time < trg.time) {
			++it;
		}
		return it;
	}
};


/**
 * The triggger Factory is stored once per gamestate and manages the Eventqueue
 *
 * pxd:
 * cppclass TriggerFactory:
 *     pass
 */
class TriggerFactory {
public:
	/** Subscribe to the event queue and manage this */
	TriggerFactory(EventQueue *queue) :
		queue(queue) {}

	/**
	 * Remove all associated events for this trigger.
	 * Does not invalidate the trigger!
	 */
	virtual void remove(Trigger *);

	/**
	 * Queue the event in the underlying event queue.
	 */
	EventQueue::Handle enqueue(Trigger *,
	                           const Trigger::EventTrigger &,
	                           const curve_time_t &now);
	EventQueue::Handle enqueue(Trigger *,
	                           const Trigger::EventTrigger &,
	                           const EventQueue::time_predictor &,
	                           const curve_time_t &now);

	/**
	 * Remove the event and insert it again in the required position.
	 * /FIXME: Only works for future events at the moment
	 */
	void reschedule(const EventQueue::Handle &,
	                const EventQueue::time_predictor &new_timer,
	                const curve_time_t &now);

	void reschedule(const EventQueue::Handle &,
	                const curve_time_t &now);

	void reschedule(const curve_time_t &now);

	std::string generate_uuid(void const *);

	EventQueue *queue;
};

class TriggerIntermediateMaster : public Trigger, public TriggerFactory {
public:
	TriggerIntermediateMaster(TriggerFactory *superFactory);
};


}} // openage::curve
