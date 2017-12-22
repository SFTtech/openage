Event Queue Magic
=====================

The Event Queue consists of Events and their Context:

```
struct Event {

/// Unique Identificator for exactly this event.
int32_t event_id;

/// Time this event should trigger
curve_time_ time_triggers;

/// Class of the event
EventClass eventclass

/// Curve Object this Event relates to
CurveObject context

std::function<void(CurveObject context, EventQueue &, curve_time_t &time) event;
};
```

Event ID
----------
The Event ID can be used to globally unique identify a specifig event.
Its time may change, but not the associated function to execute and the context of the function to call.

Even if the same kind of event in the same context will trigger multiple times, it will receive different IDs.

The EventHandle uses this ID to work with the event.

Time Trigger
----------------

The Curve time when this event shall be fired.
This logic is used to have a single dimensional monotonic way of marking physical and logical events within the game flow.

Event Class
--------------

Each event is correlated with an event class - optimally this refers always to the same method to be called in the end. Therefore the event class shall be the fully qualified pythonized name of the method to be called (e.g. `openage.curve.demo.ball_reflect_wall`).
This identifier may be used to adress the last executed event of this class or the next to be executed to update the gameflow.

Event Class, Context and aproximate time can together identify an event, and can therefore be used to reschedule it.
Therefore it is not neccessary to store all event handles inside a event triggerer.

Curve Context
----------------

The Curve this event is defined on. It will aide to re-trigger events, if their related curve section has changed - the context shall therefore be selected accordingly.
For Pong: Ball/Panel reflections, the context should be the panel and not the ball.


Event
------

The event to be called.
It receives as parameters the Context, the Queue this event is coming from (to resubmit more events) and the time this event was actually evaluated.



Extras
--------

There exist paired Events, for example in colissions. They work that way, that both contextes register a Event, but store each others event id. This allows the event that is executed first to remove the event from the other queue, that it will not be evaluated.
If nothing happens within the events, these should optimally be at the same time, only if one context is modified by a user, and the time changes, it will differ.


Internas
=========

Triggers
==========

There exist the following classes of events

1. `on_change(class)`
    this is immediately fired, whenever the context curve is changed. It can be triggered multiple times
    _Usage_: Used to implement dependencies between objects
2. `on_change_at(time, class)`:
    This is fired at `time`, if whenever the context curve was changed would fire. If `time` is already in the, the event is fired immediately. This is only fired once.
    _Usage_: the right way to implement dependencies, because they can be accumulated. This event will then be only fired once, if the same  class is given.
3. `on_pre_horizon(time, class)`:
    This is fired when `time` is inside the next rendering frame. it is only fired once.
    _Usage_: normal events interactions and stuff - this is where everything will happen.
4. `on_pass_keyframe()`:
    This is fired whenever an event passes a keyframe so that the referred keyframe is not relevant for the execution anymore.
    _Usage_: Saving a replay, memory cleanup.


Execution Queues
--------------------

Modification
---------------
The events are stored in different execution queues, one for the upcoming future and one for the recent past.
When a new event is inserted it is stored ordered in an std::queue of events that will happen in the future.

Sometimes the occurence of an event has to be re-evaluated (in Pong always when the player moves the panel. Then it has to be checked, if the player will actually hit the ball or not.).
When this happens, the updated event will receive a new time, the rest will stay the same.
it can be adressed via "in `context`: get next event of `class` relative to `time`".
These re-evaluations usually happen, when we receive user input, and therefore the future changes.

There is also the option that the reevaluation of an event leads to the conclusion, that it does not need to be executed at all.

The retriggering of when the execution will happen shall happen `on_change_at` of the associated Curve.



Dependencies
===============

Events depend on each other that they may either be not executed at all or rescheduled when some action failed.
