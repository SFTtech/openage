# Curves

*Curves* are data structures that manage changes to data over time. More precisely,
they do not only store data but time-value pairs (keyframes) that allow the retrieval
of a curve data value for any point in time.

Curves are an integral part of openage's event-based game simulation.

1. [Motivation](#motivation)
2. [Architecture](#architecture)
3. [Curve Types](#curve-types)
   1. [Primitive](#primitive)
      1. [Common Operations](#common-operations)
      2. [Discrete](#discrete)
      3. [Continuous](#continuous)
      4. [Segmented](#segmented)
   2. [Container](#container)
      1. [Queue](#queue)
      2. [Unordered Map](#unordered-map)


## Motivation

The openage curve structures are inspired by a [similar implementation](https://www.forrestthewoods.com/blog/tech_of_planetary_annihilation_chrono_cam/#.lmxbu3vld) from the game Planetary Annihilation.

Curves intend to solve synchronicity problems in games. In traditional implementations
like lockstep, data is changed incrementally in regular intervals (ticks). Keeping
these changes and the overall gamestate consistent/in sync over network or across threads
is very important, as one missing change can result in a desync of the entire simulation.
Recovering from a desync can also be very hard.

In comparison, simulation with curves allows both async operations and an easier recovery
from desync states. Data is not changed incrementally but is instead calculated using
*keyframe interpolation*. In other words, changes to data are treated as keyframes on
a timeline with in-between values being interpolated from adjacent keyframes.

![Curve keyframes example](ASDF)

Additionally, curves cannot only access values for the current simulation time but also for
any past or even future times. Keyframes can also be inserted for any point in time without
directly invalidating the state, making curves more reliable in async scenarios (although
resolving dependencies for keyframes in the past can still be challenging).

The usage of curves has a few downsides though. They are less space efficient due to the
keyframe storage, interpolation are more costly more costly than incremental changes, and
their integration is more complex than the usage of simpler data structures. However, in
situations where operations are predictable, long-lasting, and easy to calculate - which
is the case for most RTS games - these downsides are usually outweight by the positives.

## Architecture

![Curve classes hierarchy UML](ASDF)

openage provides several curve types with different interpolation behaviour that each have
their specific use case. Primitive data types are covered by the `BaseCurve` interface
and its derivates of which the `Discrete`, `Continuous`, and `Segmented` are the practically
usable types. For containers, there are classes for `Queue` and `UnorderedMap` available.
All curve types are templated, so that they can store any type of value (with some constraints).
The usable types are explained in more detail in the [Curve Types](#curve-types) section.

Keyframes are implemented as time-value pairs by the `Keyframe` class. `KeyframeContainer`
is used by all curves to manage their keyframe storage. It also provides functionality to
efficiently access and insert keyframes as wel as sorting them by time. For the time
type, the [simulation time type](/doc/code/time.md) from the `openage::time` namespace is
used.

It should be noted that curves are not useful in every situation as keyframe insertion,
interpolation, and searching keyframes based on time create significant overhead. Curves
should be used for variables or members where

* data values must be tracked over time (e.g. HP of a game entity), or
* data values are not modified/read very often (e.g. not every frame), or
* data values are supposed to be sent over the network.

This is usually the case for all game data in [game entity components](/doc/code/game_simulation/components.md)
inside the game simulation.

Curves should generally not be used for variables or members where

* data values are not tracked over time (e.g. for temporary variables), or
* data values are modified/read very often (e.g. in the rendering loop), or
* data values don't affect the simulation state (e.g. visual settings).


## Curve Types

This section provides an overview over the available curves types.

### Primitive

![Primitive curve types UML](ASDF)

Primitive curves are intended for members with single value types. These include, for example,
the primitive C++ types (e.g. `int`, `float`, `std::string`), simple structs and data classes,
and shared pointers.

On contruction of a primitive curve object, a keyframe with time `t = time::time_t::min_value()`
and the default value of the value type is inserted. This is done to ensure that for any
requested time `t`, there is always a valid value to be returned. This mirrors the expected
behaviour from declaring primitive values in C++ where members may be auto-initialized without
explicit assignment to a default value. The default value for curves can also be explicitely
assigned in the constructor. For value types that don't have default values, a default value
must always be passed in the constructor.

`BaseCurve` objects can be targeted by or trigger events from the [event system](/doc/code/event_system.md).
As a consequence, curves are not copy constructable as they require a unique ID for
event management. However, it is possible to copy the keyframes of one curve to
the other using the `sync(..)` method. `sync(..)` also works for curves with different
values types if a converter function from one value type to the other is supplied.

#### Common Operations

All primitive curves support the following operations. They may work slightly different
for specific curve types.

**Read**

| Method          | Description                                                 |
| --------------- | ----------------------------------------------------------- |
| `get(t)`        | Get (interpolated) value at time `t`                        |
| `frame(t)`      | Get the previous keyframe (time and value) before or at `t` |
| `next_frame(t)` | Get the next keyframe (time and value) after `t`            |

**Modify**

| Method                  | Description                                                                       |
| ----------------------- | --------------------------------------------------------------------------------- |
| `set_insert(t, value)`  | Insert a new keyframe value at time `t`                                           |
| `set_last(t, value)`    | Insert a new keyframe value at time `t`; delete all keyframes after time `t`      |
| `set_replace(t, value)` | Insert a new keyframe value at time `t`; remove all other keyframes with time `t` |

**Copy**

| Method           | Description                                                                            |
| ---------------- | -------------------------------------------------------------------------------------- |
| `sync(Curve, t)` | copy keyframes from `Curve`, starting at time `t`; delete all keyframes after time `t` |


#### Discrete

![Discrete curve function example]()

Discrete curves implement **constant interpolation** between keyframes. This means
that the value returned by `get(t)` is always equal to the value of the previous
keyframe.

Discrete curves should be used for values that only change at specific points in time,
e.g. for the health of a game entity.


#### Continuous

![Continuous curve function example]()

Continuous curves implement **linear interpolation** between keyframes. This means
that the value returned by `get(t)` is calculated from the value difference
between the keyframes before and after `t`. If there is no keyframe after `t`,
the value of the previous keyframe is used (like on discrete curves).

Value types on continuous curves need to implement methods for the `operator*(..)` and
`operator-(..)` operations to support linear interpolation. In particular, `operator*(..)`
must support multiplication with `time::time_t` and `operator-(..)` must support
substraction for values of the same type.

Continuous curves do not allow jumps between keyframe values (hence "continuous").
Therefore, there cannot be two keyframes inserted for the same time `t`.

Continuous curves should be used for values that change gradually over time,
e.g. a game entity's position or the construction progress of a building.


#### Segmented

![Segmented curve function example]()

Segmented curves implement **linear interpolation** between keyframes and additionally
allow jumps between keyframe values. As with continuous curves, the value returned by `get(t)`
is calculated from the value difference between the keyframes before and after `t`.

Jumps are inserted using the special methods `set_insert_jump(..)` and `set_last_jump(..)`:

| Method                               | Description                                                                       |
| ------------------------------------ | --------------------------------------------------------------------------------- |
| `set_insert_jump(t, value1, value2)` | Insert a two new keyframes at time `t`: `(t, value1)` and `(t, value2)`           |
| `set_last_jump(t, value1, value2)`   | Insert a two new keyframes at time `t` like above; delete all keyframes after `t` |

Segmented curves should be used for values that change gradually but are not on
connected intervals. Typically, this are values that wrap around at some point,
e.g. angles between 0 and 360 degrees.


### Container

Container curves are intended for storing changes to collections and containers.
The currently supported containers are `Queue` and `UnorderedMap`.

The most important distinction between regular C++ containers and curve containers
is that curve containers keep track of when modifications happen and what changes
to an element are made. Deleting elements also does not erase elements from memory.
Instead, they are simply hidden for requests for time `t1` after the deletion time `t2` if
`t1 > t2`.


#### Queue

Queue curve containers store elements in first-in-first-out (FIFO) insertion order
while additionally keeping track of element insertion time. Requests for the front element
at time `t` will return the element that is in front of the queue at that time.
The queue can also be iterated over for a specific time `t` which allows access to
all elements that were in the queue at time `t`.

#### Unordered Map

Unordered map curve containers store key-value pairs while additionally keeping
track of element insertion time. Requests for a key `k` at time `t` will return the value
of `k` at that time. The unordered map can also be iterated over for a specific time `t` which
allows access to all key-value pairs that were in the map at time `t`.
