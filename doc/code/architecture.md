# openage architecture

openage is separated into many modules ("subsystems").

1. [Overview](#overview)
   1. [Components](#components)
   2. [Utilities](#utilities)
2. [Information flow](#information-flow)
   1. [Current architecture](#current-architecture)
   2. [Goal architecture](#goal-architecture)


## Overview

Some of the components are already implemented, others are not.
All of them need to be revisited to implement the goal architecture.


### Components

* Audio system
* Configuration system
* [Coordinate system](coordinate-systems.md)
* [Converter](converter/)
* [Input system](input/)
* Networking
* [Game data database](/doc/nyan/)
* [Rendering](renderer/)
* [Simulation](game_simulation/)
* [Time management](time.md)
* [User interface](gui.md)


### Utilities

* Datastructures
  * [Curves](curves.md)
* [Error handling](exceptions.md)
* Filesystem abstraction
* Job dispatching
* Live reloading
* [Logging system](logger.md)
* [Python interface](pyinterface.md)
* Random number generator
* [Test and demo infrastructure](testing.md)


## Information flow

### Current architecture

The current data flow of openage is just to display the raw simulation data.

```
input -> simulation -> renderer -> output
```


### Goal architecture

```
0 input ->
1 network ->
2 simulation ->
3 network ->
4 prediction/interpolation ->
5 renderer ->
6 output
```

For *singleplayer* we could bypass `1 network` and `3 network` by using direct function calls or talk to a local socket.
For *multiplayer* this means that we will have a single authoritative server that is just running
the simulation. Each client then receives the data visible for it.

The current architecture can be extended by adding the missing components in
between.

The `prediction/interpolation` reuses the `simulation` code, but is
non-authoritative: The data provided from `2` has higher priority.

There exists a link of `0 input -> 4 prediction` so that input
is immediately accounted in the prediction, thus displayed.
