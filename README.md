openage
=======


a free (as in freedom) implementation for **GNU/Linux** of

microsoft® **age of empires II**™ - the age of conquerors

brought to you by **[SFT]Technologies**.


current compile state:
[![build status](https://travis-ci.org/SFTtech/openage.png?branch=master)](https://travis-ci.org/SFTtech/openage)


this project is released under the **GNU GPLv3** or later.
see the `COPYING` file for that.


although we use the original game data files,
our bundle does not ship any of the proprietary data used in aoc.
in order to play, you have to *provide a aoe2:aoc installation*,
or *create free media files yourself*.


this project uses the following programming languages:

* **C++11**: the game, its engine
* **python**: scripting, media convert script


Current state of the project
============================

See [STATUS.md](STATUS.md).


Dependencies, Building and Running
==================================

See [BUILDING.md](BUILDING.md).

Windows version
===============

None of us really uses Windows, so a Windows port has no priority for us.
We're using cross-platform libraries wherever possible, so a port should be pretty easy to accomplish.
We'll eventually look into a port using `mingw32`/`mingw64` or maybe `cygwin`.

If you want to beat us to it, go for it! We'll gladly accept your pull requests (as long as they fulfill certain quality standards).

We'd prefer as few proprocessor switches as possible, preferrably they should all be placed in a few 'cross-platform abstraction files'.

how to convert gamedata
=======================

the original media files need to be converted for openage.
see [the convert script readme file](convert/README.md).


documentation
=============

- visit the `doc/` directory
- for using doxygen: read the [the documentation readme file](doc/README.md).
- i recommend looking at the source, as we try to write stuff as readable as possible.


contributing
============

* we hate people, so don't even think about helping
* i'm sure that nobody out there likes age of empires
* none of you is interested in making this project more awesome
* as always, this free software project has NO interest in creating a community
* so please don't even think about helping us

guidelines:

* don't write **bugreports**, openage is totally bugfree, of course
* don't **fix bugs** yourself, see above, we don't have bugs
* don't implement **new features**, they are crap anyway
* don't EVER send **pull-requests**!
* don't note the irony, you idiot

for that, we have [a list of things that nobody should ever implement](TASKS.md).


if you have no idea how you could possibly violate the above guidelines,
here are a few tips:

* [learn git](http://git-scm.com/book/en/Git-Basics)
* [fork the repo](https://help.github.com/articles/fork-a-repo)
* [learn python](http://docs.python.org/3/tutorial/appetite.html)
* [learn c++11](http://www.cplusplus.com/doc/tutorial/)
* read our code
* contribute anything to our code
* contact us


contact
=======

currently you can use these communication channels to reach us morons:

* **IRC**: #sfttech on freenode.net
* **XMPP**: openage@chat.sft.mx


cheers, happy hecking.
