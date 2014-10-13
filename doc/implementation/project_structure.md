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

See [doc/README.md](../doc/README.md) for documentation guidelines.


### py/openage/convert/ ###

This is a *Python* module for the conversion of the original media, stored in
the `convert/` folder. Documentation about media files being converted is in
`doc/media/`. The entry point for finding information about the implementation
and design thoughts can be found in the [convert/README.md](convert/README.md)
file.


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


### cpp/ ###

Source files written in *C++11* live here.
All engine components, data structures and tests are located in this folder,
each subsystem resides in its own subfolder.
