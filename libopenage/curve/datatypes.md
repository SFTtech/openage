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

TUBE SERIALIZATION
==================

Serialization condenses data into change sets:

Repeat the following blob:
+-----------------------------------------------------------+
| ID (24Bit)                                                |
| flags (delete, del_after, time, time2, data, add) (8Bit)  | # In the first quadruple it is stored which data fields are set.
| if (flag & time) time1                                    | # In the second quadruple the usage of the data is stored
| if (flag & time2) time2                                   | # | time | time2 | data | UNUSED | delete | add | del_after | UNUSED |
| if (flag & data) keyframe: size(16) | data                |
+-----------------------------------------------------------+

Meaning of Flags
----------------

== DELETE ==

After DELETE it is allowed to reuse the ID
When no Time is defined, then the deletion is "now", if TIME1 is defined, then the element will be deleted at this time.

== ADD ==

Create a new element with the given ID. Add has to have at least TIME1 and DATA set.

== DEL_AFTER ==

Delete all keyframes after TIME.

== TIME1 ==

Set the Keyframe time or the creation time of an element

== TIME2 ==

Set the Destruction time of a container element

== DATA ==

The Keyframe data prefixed by data length



Serialization of keyframes for different data types
----------------------------------------------------

Simple types: Binary Serialization of the data types, interpolation mode does not matter

Containers
For Containers DELETE_AFTER is not supported.

== Map ==

Store TIME2 as death time - if the element has a death time yet.
The ID of the object is submitted at creation as its own curve.

== Set ==

This container is simple to store only times (birth (TIME1)  and death (TIME2) of each unit) and only update the keyframe data when neccesary

== Queue ==

Elements here have only one single time, so TIME2 is not used.
They can be created with ADD and removed with DELETE.
