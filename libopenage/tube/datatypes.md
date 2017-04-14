TUBE DATATYPES
=================

This document describes the datatypes that should be available within the tubes library.
They consists of simple, single dimensinal types and list types.

This document is intended for brainstorming on needed datatypes.

Simple Types
------------

Simple types only have one distinct value at a specific point in time.

Discrete Interpolation
----------------------

"Step function" style values. These types adopt the new value exactly at the point in time changed.
They are useful for example for values like unit capacity, hitpoints, resource count, ...

Linear Interpolation
--------------------

Linear connections between two points. These types have to overload the operators + and *, since these
are used to interpolate between t\_n and t\_n+1. These values change consistently over time, if t\_n+1 exists.
These are useful for example for unit position, building progress, ...

Nyan Values
--------------

This container keeps track of nyan objects over time and their respective properties.

Container Types
===============

Container types hold lists of items at any given time. They store a creation and a deletion timestamp for each item, and offer iteration logic to traverse the active elements in the container.

Map Container
-------------

The map container stores items based on an unique identifier mapped to an item. It keeps track of the existence of the item. No guaranteed order of items within this container is given (similar to std::unordered_map).
This container is useful for example for unit lists ...

Set Container
----------------

The set container stores items, just as a normal array would. It keeps track of the existence of the items, but does not guarentee any particular ordering (similar to std::unordered_set). 
This Container is useful for any non-indexed data structures, for example projectiles.

Queue Container
---------------

The queue container represents a random access queue while keeping the ordering of the queue.
It is usually used for pushing in the back and popping at the front (FIFO-Stlye) but offers random access insertion and deletion as well.
This container is useful for example for action queues and buildung queues.

TUBE FILES
============

