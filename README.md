openage
=======


a free (as in freedom) implementation of

microsoft(r) **age of empires II** (tm) - the age of conquerors


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
	cmake
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
	cd data && ../bin/openage

for other make targets, see:

	make help

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

the original aoc game data format is, lets say, a challenge to use.
therefore it has to be converted in order to be usable with openage.

the python script for that is in the `convert/` folder, it will convert all the
media files to formats being usable by openage.

this also means that you have to own the original media files, as we are way
too lazy to create free media files for now, that's something the community can do..


to convert only media files we need at the moment, execute this:

	make media AGE2DIR="~/.wine-age/drive_c/programs/ms-games/aoe2"

the above command is an alias for the following python call:

	python3 convert -o . $PATH_TO_YOUR_AOC_INSTALLATION $(make medialist)


to convert ALL original media files to openage format, execute the following:

	python3 convert -o . $PATH_TO_YOUR_AOC_INSTALLATION

you will then find the converted files in `./data/age/`
you probably don't want to convert all the files (we don't use them yet..),
the conversion takes some time, so be patient.


try

	python3 convert --help

to see the convert script usage.


documentation
=============

static docs
-----------

general documentation files reside in the `doc/` directory.
you can find ideas, milestones, planning and workflow descriptions etc in there.

a good entry point may be the file `doc/implementation/project_structure`
as it roughly explains where to find what code in the project.

dynamic docs
------------

dynamic documentation files are generated from comments in the source code, using doxygen.
the dynamic docs tend to only describe stuff you could also understand by reading the code itself,
so don't expect too much, use the static docs instead.

after you configured the project, you can invoke

	make doc

to create doxygen html and LaTeX files.


after creation, view them in a browser by

	$(browser) bin/doc/html/index.html


or, if you want to create LaTeX documents:

	make -C bin/doc/latex/ pdf
	$(pdfviewer) bin/doc/latex/refman.pdf


**i recommend looking at the source, as we try to write stuff as readable as possible.**


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

For that, we have [a list of things that nobody should ever implement](TASKS.md).

contact
=======

currently you can use these communication channels to reach us morons:

* **IRC**: #sfttech on freenode.net
* **XMPP**: openage@chat.sft.mx


cheers, happy hecking.
