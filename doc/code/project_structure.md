# Openage project structure #

One of the biggest problems for newcomers who want to contribute code to free
software projects is that they have no idea where to start. Reading,
understanding and finding the relevant code part is hard. This file will help
you understanding the modular structure of this project.


## Folders ##

Each folder in the root of the project contains fundamentally different
contents. This ensures separation of components and defines the base structure
of the project.


### doc/ ###

You may not believe it, but documentation stuff is in the `doc/` folder. For
example, this document resides in there, because it documents something. wow.

See [doc/README.md](/doc/README.md) for documentation guidelines.


### openage/ ###

Contains the Python components of openage, including
 - the main game entry point
 - testing code
 - code generation
 - asset conversion
  - see `doc/media/`
  - see [doc/code/convert/README.md](/doc/code/convert/README.md)


### assets/ ###

Game assets required at run time are placed in here.
This includes shaders etc.
The directory is installed to /usr/share/openage


### userassets/ ###


Dynamically created assets, such as original game files.
This directory is empty after cleaning the repository and installing.
It is located in ~/.userassets for an installed game.


### buildsystem/ ###


Buildsystem components, namely `cmake` scripts and configuration templates,
are placed in this folder. This includes scripts for test integration, python
module definition, source file gathering and executable definitions are placed
in this directory.

The code compliance checker also lives here.


### cpp/ ###

Source files written in *C++14* live here.
All engine components, data structures and tests are located in this folder,
each subsystem resides in its own subfolder.
