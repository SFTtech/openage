# Project Structure

One of the biggest problems for newcomers who want to contribute code to free
software projects is that they have no idea where to start.

Reading, understanding and finding the relevant code part is hard.

This file explains the modular structure of the project.

1. [Architecture](#architecture)
2. [Languages](#languages)
3. [Folders](#folders)
   1. [assets/](#assets)
   2. [buildsystem/](#buildsystem)
   3. [cfg/](#cfg)
   4. [dist/](#dist)
   5. [doc/](#doc)
   6. [etc/](#etc)
   7. [legal/](#legal)
   8. [libopenage/](#libopenage)
   9. [openage/](#openage)
   10. [packaging/](#packaging)



## Architecture

The [overall architecture](/doc/code/architecture.md) describes the conceptual overview.


## Languages

We use Python, Cython and C++.

| Extension     | Language | Usage                                                 |
| ------------- | -------- | ----------------------------------------------------- |
| `.py`         | Python   | Everything that does not crunch data                  |
| `.pyx` `.pxd` | Cython   | Fast Python code, glue between C/C++ and Python       |
| `.h`   `.cpp` | C++      | Data crunching code: simulation, graphics, sound, ... |


## Folders

Each folder in the root of the project contains fundamentally different
contents. This ensures separation of components and defines the base structure
of the project.


### assets/

Game assets required at run time are placed in here. This includes everything
that is converted from the original assets (see [asset conversion](media_convert.md))
and other input like shaders etc. The directory is `assets` for local builds or
installed to `/usr/share/openage/assets` or whatever the platform recommends.


### buildsystem/

Buildsystem components, namely `cmake` scripts and configuration
templates, are placed in this folder. This includes scripts for test
integration, python module definition, source file gathering and
executable definitions are placed in this directory.

The code compliance checker also lives here.


### cfg/

Contains the standard configuration of the engine, e.g. keybindings.


### dist/

Files for package distribution.


### doc/

In the `doc/` folder is conceptual documentation, ideas, algorithms, etc.
The code itself is commented with `/** doxygen comments */`.

See [doc/README.md](/doc/README.md) for documentation guidelines.

### etc/

Additional resources for development tools, e.g. pylint and valgrind configs.


### legal/

Our license (GPLv3+) and licenses of other projects' components that are
integrated into openage.


### libopenage/

Source files written in **C++20** live here.
All engine components, data structures and C++ tests are located in this
folder, each subsystem resides in its own subfolder.


### openage/

Contains the Python3 auxiliary components of openage.
Python3 is used for non-data-crunching tasks, like converting original assets,
scripting and modding.

The main entry point for openage is in `__main__.py`.


### packaging/

CMake script for creating a package for distribution.
