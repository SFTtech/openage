# Event System

Game simulation in openage is event-driven. The internal event system
manages the creation, scheduling, and execution of these events.

1. [Architecture](#architecture)
2. [Event Handler Setup](#event-handler-setup)
3. [Event Instances](#event-instances)
4. [Triggering/Targeting Events](#triggeringtargeting-events)
5. [Canceling Events](#canceling-events)


## Architecture

![event system UML](images/event_classes_uml.svg)

The central component of the event system is the *event loop* (`EventLoop`). It is
the main point of interaction between the game simulation logic and the event system.
New events (`Event` instances) are added via the `create_event(..)` method and scheduled based on the
configuration of the associated *[event handler](#event-handler-setup)* (`EventHandler`).
Internally, the event is stored inside an *event queue* (`EventQueue`) instance
sorts the events by invoke time.

Events are executed by passing the current [simulation time](time.md) to the
event loop's `reach_time(..)` method. This executes all not-yet-executed events
until the given target time stamp

```
t_old < t_e <= t_cur
```

where `t_cur` is the current simulation time, `t_old` is the last time `reach_time(..)` was
called, and `t_e` is the event's scheduled invoke time. Events are executed in order of
earliest to latest invoke time. During execution, events may trigger the rescheduling of
other events if there are dependencies between the events.

Both the creation and execution of events require passing a `State` object that functions
as a persistant global storage for values that are not linked to a specific event executions.
In practice, `State` is primarily used to pass the state of the [game simulation](/doc/code/game_simulation/)
to the event system via an object of the derived `GameState` class. This object allows
access to the indexing structures of the current game that can be used to retrieve
the available game entities, for example.


## Event Handler Setup

Event effects on the game simulation are implemented by *event handlers*. Event handlers
are attached to the event on creation and are invoked when the event is executed by the event
loop. They are also used for (re-)scheduling the invocation time of the event.

Event handlers are implemented as derivatives of the `EventHandler` class. Event handlers
must implement 3 methods:

* `invoke(..)`: Called by the event loop when the event is executed.
* `predict_invoke_time(..)`: Called when the event handler shall determine a new invocation time of an event,
                             i.e. (re)place it on the event queue
* `setup_event(..)`: Sets up dependencies during creation of the event.

Additionally, event handlers require a unique ID (defined as a string) as well as
a *trigger type* which defines under which circumstances an event is executed.
These parameters can be defined at runtime when instantiating the event handler.

New events require that an event handler object is attached during creation. This
can be done by passing the event handler object to the `create_event(..)` method.
If the event handler should be reusable, it can also be registered on the event loop
via the `add_event_handler(..)` method. When calling `create_event(..)` on the event
loop to create a new event, the event handler can then be referenced by a unique
ID string.


## Event Instances

When calling `create_event(..)` on the event loop, an `Event` instance is returned which
allows access to the configuration parameters of the event. Event instances store
references to:

* **Event target**: Entity targeted by the event, i.e. the event entity that the event effects should be applied on. Implemented as a weak reference to let the target expire.
* **Event handler**: Event handler object used for rescheduling and executing the event.
* **Invoke time**: Determines when the event is executed.
* **Time of last change**: Time of the last change to invocation time. Used for rescheduling the event.
* **Event payload**: Optional map of parameters passed to the event handler on invocation.

These parameters may be changed after creating the event. Alternatively, it is possible to
[cancel](#canceling-events) the event and create a new event with the updated parameters.


## Triggering/Targeting Events

To allow an event to modify an object in its invocation, the object's class must be
a derivative of the `EventEntity` class. By inheriting from `EventEntity`, a class can
be targeted by events, trigger or change events, or both.

An event target is passed during the creation of an event (i.e. in `create_event(..)`)
and later when executing the event via the event handler's `invoke(..)` method. Events
can only have one target at a time.

`EventEntity` derivatives may also be used to trigger or reschedule the invocation of events. To do this,
the event has to be added as a dependent for the event entity using the `add_dependent(..)`
method of `EventEntity`. Calling the `changes(..)` method of `EventEntity` will then result
in a rescheduling of the event in the next iteration of the event loop.


## Canceling Events

Events are automatically canceled if their targeted event entity has expired, i.e. it has been deleted
from memory. To accomplish this target references are implemented using `std::weak_ptr`
which are checked for expiration before event execution.

Manually canceling events can also be done by calling `cancel(..)` on an `Event` instance.
In the current implementation, this change is effective immediately, even if the provided
reference time is in the future.
