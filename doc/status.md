openage status
==============

This file contains the current feature set of openage.


The list must grow
------------------

As features don't create and add themselves down there,
you can make it happen! See the [contribution intro](/doc/contributing.md) how to add stuff.

Please update this file accordingly when adding features.


User-Visible Features
---------------------

* Convert all original media files
* Flat, infinite terrain rendering
* Basic map generation

* Interactive menus with on-screen guide
  * Construct mode: Terrain and unit placement
  * Action mode: Unit control and gameplay!
    * Shortkeys for building creation
    * Attacking, woodchopping, gathering, ...
    * Change active player with number keys

* Map scrolling with arrow keys or middle mouse button

* Screenshots with `F2` -> `/tmp/openage_2019-12-31_23-59-59_xx.png`

* Ingame console
  * Open with `backtick`

* Integrated profiler: `F12` to start/stop + show results


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
