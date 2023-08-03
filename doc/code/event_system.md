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

Events are executed by passing the current [simulation time](/doc/code/time.md) to the
event loop's `reach_time(..)` method. This executes all events with invoke time

```
t_old < t_e <= t_cur
```

where `t_cur` is the current simulation time, `t_old` is the last time `reach_time(..)` was
called, and `t_e` is the event's scheduled invoke time. Events are executed in order of
earliest to latest invoke time. During execution, events may trigger the rescheduling of
other events if there are dependencies between the events.

Both the creation and execution of events require passing a `State` object that functions
as a persistant global storage for values that are not linked to a specific event executions.
In practice, `State` is primarily used to pass the state of the [game simulation](/doc/code/game_simulation/README.md)
to the event system via an object of the derived `GameState` class. This object allows
access to the indexing structures of the current game that can be used to retrieve
the available game entities, for example.


## Event Handler Setup

The logic for (re-)scheduling and invoking an event is implemented by derivatives of the
`EventHandler` class. Event handlers must implement 3 methods:

* `invoke(..)`: Called by the event loop when the event is executed.
* `predict_invoke_time(..)`: Calculates the desired invoke time. Called when the event is first created and whenever
                           the event is due to be rescheduled (e.g. if a dependency event changes).
* `setup_event(..)`: Sets up dependencies during creation of the event.

Additionally, event handlers require a unique ID (defined as a string) as well as
a *trigger type* which defines under which circumstances an event is executed.
These parameters can be defined at runtime when instantiating the event handler.

To enable events to use the event handler, an event handler object must first
be initialized and then passed to the event loop. This can either be done explicitly via
the `add_event_handler(..)` method of the `EventLoop` class or during the
creation of an event via an overloaded `create_event(..)` method. Once an event
handler is registered, new events can also be created using the event handler's
unique ID string when calling `create_event(..)`.


## Event Instances

When calling `create_event(..)` on the event loop, an `Event` instance is returned which
allows access to the configuration parameters of the event. Event instances store
references to:

* **Event payload**: Map of parameters passed to the event handler on invocation.
* **Event target**: Entity targeted by the event. Implemented as a weak reference to let the target expire.
* **Event handler**: Event handler object used for rescheduling and executing the event.
* **Invoke time**: Determines when the event is executed.
* **Time of last change**: Time of the last change to invocation time. Used for rescheduling the event.

These parameters may be changed after creating the event. However, in most cases it's
more beneficial to create a new event instead and [cancel](#canceling-events) the old
event.

## Triggering/Targeting Events

Both sending and receiving events is managed by the same interface, i.e. the `EventEntity`
class. By inheriting from `EventEntity`, a class can be targeted by events, trigger
or change events or both.

An event target is passed during the creation of an event (i.e. in `create_event(..)`)
and later when executing the event via the event handler's `invoke(..)` method. Events
can only have one target at a time.

`EventEntity` derivatives may also be used to trigger or reschedule the invocation of events. To do this,
the event has to be added as a dependent for the event entity using the `add_dependent(..)`
method of `EventEntity`. Calling the `changes(..)` method of `EventEntity` will then result
in a rescheduling of the event in the next iteration of the event loop.


## Canceling Events

Events are automatically canceled if their target has expired, i.e. it has been deleted
from memory. To accomplish this target references are implemented using `std::weak_ptr`
which are checked for expiration before event execution.

Manually canceling events can also be done by calling `cancel(..)` on an `Event` instance.
In the current implementation, this change is effective immediately, even if the provided
reference time is in the future.
