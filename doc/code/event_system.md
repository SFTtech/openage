# Event System

- event system in openage
  - sending and receiving events
  - handling events
  - integration of events into the simulation

1. [Architecture](#architecture)


## Architecture

![event system UML]()

- central component: event loop
  - main point of interaction to create events
  - manage event queue and event store
  - manage interdependencies between events
  - events are based on simulation time
    - time from time loop and its clock
    - advance to a point in time
    - check for interdendency changes and reorder events if necessary
    - execute event handlers in order of execution time

- event
  - represents an event
  - always targets an event entity
  - executed at specific time which can be modified by event loop based on dependencies
  - stores parameters for execution

- event entity
  - inherit from this class
  - target OR dependency of event
  - as target:
    - may be manipulated by event handler on event execution
  - as dependency:
    - triggers the event execution

- event handler
  - executes if event fires
  - can be registered on event loop (if reusable)
    - or passed when creating event
  - trigger type of handler manages when event is executed
    - dependency, once, repeating, etc.

- state
  - object that persists across event executions
  - can be anything, usually used for global vars
