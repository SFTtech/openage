# Curves

- data structure for keyframe-based simulation
  - not just data but manages data over time


1. [Motivation](#motivation)
2. [Architecture](#architecture)
3. [Curve Types](#curve-types)
   1. [Primitive](#primitive)
      1. [Discrete](#discrete)
      2. [Continuous](#continuous)
      3. [Segmented](#segmented)
   2. [Container](#container)
      1. [Queue](#queue)
      2. [Unordered Map](#unordered-map)



## Motivation

- inspired by a similar implementation for Planetary Annihilation
  - https://www.forrestthewoods.com/blog/tech_of_planetary_annihilation_chrono_cam/#.lmxbu3vld
- problem: data is hard to keep consistent in sync (synchronous)
  - over network
  - across threads
  - state should be the same for all components
- solution: instead of only storing *current* data, store as keyframes with (data, time)
  - storage becomes a function that tracks data values over time -> timeline
  - data in between keyframes can be interpolated
  - changing data can be asynchronous -> keyframes can be added for future
  - reverting to previous simulation time becomes much easier
- downsides:
  - requires more storage
  - interpolation is more costly than simple ticks
  - can be more complex to integrate
- however:
  - RTS actions are often predictable and therefore doesn't require many keyframes
  - data doesn't have to be accessed only when necessary
  - may be easier to understand in implemention

## Architecture

- major components
  - BaseCurve: primitive data
    - Discrete
    - Interpolated
      - Continuous
      - Segmented
  - Queue and UnorderedMap: containers
  - Keyframe: time-value pair
  - KeyframeContainer: storage for curve keyframes
  - curve::time_t: time type -> fixed point that represents simulation time in seconds

- which curves should be used for
  - data that isn't modified or read
  - data that must be tracked over time
    - ALL game data in game entity components that is required for simulation

- what curves should not be used for
  - data that is modified or read often
  - data that doesn't have to be tracked over time
    - e.g. temporary variables
    - e.g. things that don't affect the simulation like visual settings

## Curve Types

- overview of built-in curve types

### Primitive

- curves for storing the change for primitive/single values
  - primitive C++ types, e.g. int/float/std::string
  - objects
  - shared ptr

- default value is assigned for t = curve::time_t::min
  - guarantees that there is always a value to access
  - mirrors expected behaviour from declaring primitive values in C++
  - for objects that don't have default constructor, the default must be assigned when initializing curve

- operations
  - read
    - get(t): Get the value at time t
    - frame(t): Get the previous keyframe (time and value) before t
    - next_frame(t): Get the next keyframe (time and value) after t
  - modify
    - set_insert(t, value): Insert a new keyframe value at t
    - set_last(t, value): Insert a new keyframe value at t, delete all keyframes with after t
    - set_replace(t, value): Insert a new keyframe value at t, remove all other keyframes with time t
  - copy
    - sync(Curve, t): copy keyframes from one curve to the other, starting at t

#### Discrete

- value between two keyframes is constant
  - returns value of previous keyframe

- used for values that dont change much
  - e.g. current health

#### Continuous

- value between two keyframes is interpolated
  - returns interpolated value from requested time between t1 and t2
  - if t > last keyframe, then behave like discrete curve
  - currently only supports linear interpolation

- used for values that change gradually
  - e.g. entity position, current construction progress of building

#### Segmented

- value between two keyframes is interpolated
  - behaves like continuous but allows "jumps" between intervals
  - e.g. intervals are not connected like on continuous curve
  - mix between continuous and discrete

- used for values that change gradually but are not on connected intervals
  - e.g. angles (because the value jumps when rolling over from 0 to 360 degrees)

### Container

- curves for storing the changes for collections and containers
  - queues
  - maps

- most important distinction between normal containers and curve containers
  - modify operations do not erase/overwrite previous elements
  - elements are always valid at a specific time
  - if element e is erased at t, requesting it at t1>=t will return nothing but requesting it at t2 smaller t will return e

#### Queue

- stores elements in insertion order and additionally insertion time
  - front: for requested time t, return the element that was in front at t
  - iterate: iterate over queue beginning at time t
  - no empty check because elements are not deleted
    - callee must remember the iterator to the last element it accessed!

#### Unordered Map

- stores key-value pairs and remembers inserion time
  - at: return the element for key k that was present at time t
  - insert: add pair k, e at time t
  - insert: add pair k, e at time t1 and erase at t2
  - kill: erase element at time t
  - iterate over elements between time t1 and t2
