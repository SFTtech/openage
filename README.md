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


current state
=============

the current functionality state of this project can be
looked up in the [current STATUS](STATUS.md) file.


dependencies
============

	python3
	python3 imaging library (PIL) -> pillow
	opengl >=2.1
	glew
	ftgl
	dejavu font
	freetype2
	fontconfig
	cmake >=2.8.10
	sdl2
	sdl2_image
	gcc >=4.8 or clang >=3.3

	age of empires II
		conquerors expansion
		patch 1.0c
		optionally: with "userpatch"
		installed: with wine OR as the program directory


how to compile && run
=====================

on POSIX (GNU/Linux, BSD, etc..):
---------------------------------

	./configure --mode=release
	make
	make media AGE2DIR="~/.wine-age/drive_c/programs/ms-games/aoe2"
	./bin/openage --data ./data

for other make targets, see:

	make help

for startup options, call:

	./bin/openage --help

on WINDOWS:
-----------

we don't have any interest in porting the game,
but we are using cross-platform libraries wherever possible,
so porting should not be a big issue.


if you are keen on porting the game, go for it.
there should be as few preprocessor switches as possible;
if possible they should all be concentrated in one 'cross-platform abstraction file'
we suggest using either cygwin-g++ or mingw32-g++ as the windows compiler
cygwin will probably be the least effort.


we'd be happy to accept your port into our official repository,
but note that it should fulfill certain quality standards.


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
* programm whatever you want
* contact us


contact
=======

currently you can use these communication channels to reach us morons:

* **IRC**: #sfttech on freenode.net
* **XMPP**: openage@chat.sft.mx


cheers, happy hecking.
