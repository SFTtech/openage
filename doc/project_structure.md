Project Structure
=================


One of the biggest problems for newcomers who want to contribute code to free
software projects is that they have no idea where to start.

Reading, understanding and finding the relevant code part is hard.

This file explains the modular structure of the project.


## Architecture

The [overall architecture](/doc/architecture.md) describes the conceptual overview.


## Languages

We use Python, Cython and C++.

Extension     | Language  | Usage
--------------|-----------|---------
`.py`         | Python    | Everything that does not crunch data
`.pyx` `.pxd` | Cython    | Fast Python code, glue between C++ and Python
`.h`   `.cpp` | C++       | Data crunching code: simulation, graphics, sound, ...


If in doubt what to choose, use Python.


## Folders

Each folder in the root of the project contains fundamentally different
contents. This ensures separation of components and defines the base structure
of the project.


### doc/

In the `doc/` folder is conceptual documentation, ideas, algorithms, etc.
The code itself is commented with `/** doxygen comments */`.

See [doc/README.md](/doc/README.md) for documentation guidelines.


### openage/

Contains the Python3 components of openage.
Python3 is used for non-data-crunching tasks, like scripting and modding.

The main entry point for openage is in `__main__.py`. From there, other
components like asset conversion and the main game are started.

The other Python/Cython components like `convert`, `game` or ASDF


### assets/

Game assets required at run time are placed in here. This includes shaders
etc. The directory is installed to `/usr/share/openage/assets` or whatever
the platform recommends.


### buildsystem/

Buildsystem components, namely `cmake` scripts and configuration
templates, are placed in this folder. This includes scripts for test
integration, python module definition, source file gathering and
executable definitions are placed in this directory.

The code compliance checker also lives here.


### libopenage/

Source files written in **C++14** live here.
All engine components, data structures and C++ tests are located in this
folder, each subsystem resides in its own subfolder.
