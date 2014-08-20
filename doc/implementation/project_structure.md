Openage project structure
=========================

One of the biggest problems for newcomers who want to contribute code to free software projects is that they have no idea where to start.

Reading, understanding and finding the relevant code part is hard.

This file will help you understanding the modular structure of this project.


(If you plan to add things here, remember: this file shall guide newcomers!)


Folders
=======

doc/
----
You may not believe it, but documentation stuff is in the doc folder.
For example, this document resides in there, because it documents something. wow.


convert/
--------
The gamedata convert script written in Python resides in there.
Documentation about media files being converted is in `doc/media/`.


data/
-----
Shaders and converted media files are stored in here.
These are all the files being needed at run time.


src/
----
Game source files written in conservative C++11 live here.

~~src/openage: game specific code  
src/engine: engine specific code.

separation between engine and game is not clear yet, most code stays in engine until it seems to be
game specific..~~


This file is never complete!
============================

If you come up with something that may be helping others, please add it to this file!
