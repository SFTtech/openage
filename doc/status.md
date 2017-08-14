openage status
==============

This file contains the current feature set of openage.

You can follow development progress on:
* [Our blog at https://blog.openage.sft.mx](https://blog.openage.sft.mx)
* Our [/r/openage subreddit](https://reddit.com/r/openage)


The list must grow
------------------

As features don't create and add themselves down there,
you can make it happen! See the [contribution intro](/doc/contributing.md) how to add stuff.


Working features
----------------

* Original asset conversion
* Arbitrary resolution user interface
* Unit movement
* Building construction
* Resource collection
* Basic combat


Missing functionality
---------------------

* Efficient rendering
* Good pathfinding
* Networking
* [Nyan](https://github.com/SFTtech/nyan) integration
* 3D terrain
* Random map generation
* AI scripting
* Python modding API


Under the hood
--------------

There are some openage internals that may be of interest
but would go unnoted otherwise:

#### Python interface

Powered by [cython](http://cython.org/), we have an easy to use bidirectional
Python <-> C++ interface that even translates Exceptions between both.
See the [howto](/doc/code/pyinterface.md).


#### Inotify file reloading

You can edit any texture with your favorite image manipulation software.
When you save the changes, they'll appear in-game instantly.


#### Terminal emulator

We have an integrated [terminal emulator](/libopenage/console) supporting ecma-48.
You can run `vim` or anything else within openage.
This is neat to interactively edit scripts.


#### Buildsystem

Powered mainly by CMake and Python, our [buildsystem](/buildsystem) is dynamically
rebuilding and regenerating only the files it needs to.


#### Job management

Background tasks can be submitted and detached by our [job subsystem](/libopenage/job).
Used for sound decoding and various other computations.
