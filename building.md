# Building the project

This file should assist you in compiling and running the game.


## Buildsystem Design

*openage* currently consists of a pure C++ binary and the
`openage.convert` python package which in turn includes C++ extension
modules.  `openage.convert` is used to generate parts of the C++
binary code.

To actually run *openage*, it needs original game assets that need to
be extracted by the `openage.convert` python module from an
*Microsoft Age of Empires 2* installation directory (Support for
setup CDs is almost finished).

We use the `cmake` system for all our building needs. The `configure`
script is a cmake wrapper that will create a build directory and
invoke cmake with the appropriate flags. The Makefile in the project
root acts as a wrapper around several useful features.

For more build system internals, see [doc/buildsystem](doc/buildsystem).


## Dependencies

Dependencies are needed for:

* C = compiling
* R = running
* T = media convert script

Dependency list:

    CRT   python >=3.3
    T     python imaging library (PIL) -> pillow
    T     numpy
    CR    opengl >=2.1
    CR    glew
    CR    ftgl
    R     dejavu font
    CR    freetype2
    CR    fontconfig
    C     cmake >=2.8.10
    CR    sdl2
    CR    sdl2_image
    CR    opusfile
    T     opus-tools
    C     gcc >=4.8 or clang >=3.3

    T age of empires II conquerors expansion patch 1.0c
              optionally: with userpatch/forgotten empires
              expansion installed: with wine OR as the program
              directory we will support more patchlevels in the
              future.  due to fundamental technical differences,
              age of empires II HD does _not_ work yet.


## Build procedure

### Prerequisite steps for Ubuntu users (Ubuntu 14.10)

 - `sudo apt-get update`
 - `sudo apt-get install cmake libfreetype6-dev python3-dev libglew-dev libsdl2-dev ftgl-dev  libsdl2-image-dev libopusfile-dev libopus-dev libvorbis-dev libfontconfig1-dev`

### For developers/users who want to try the project

 - (obviously) clone this repo or acquire the sources some other way
 - make sure you have everything from the dependency list below
 - `./configure --mode=debug --compiler=clang++` will prepare building
 - `make` will do code generation, build all python modules and the
   openage executable
 - `make media AGE2DIR="~/.wine/drive_c/age2"` will convert all media
   files from the given age2 install folder, storing them in
   `./userassets`
 - `make run` or `./openage` will run the game. try
   `./openage --help`
 - `make test` will run the unit tests


### For installing on your local system

 - `./configure --mode=release --compiler=g++ --prefix=/usr`
 - `make` to compile the project
 - `make install` will install the binary to /usr/bin/openage, python
   packages to `/usr/lib/python...`, static assets to `/usr/share`
   etc.
 - after installing, the user will still need to run the convert
   script using `python3 -m openage.convert`, to store the convert
   original assets to `~/.openage`. This does not work yet, and the
   convert invocation will later be integrated into the main binary.


### For packagers

 - Don't use `./configure`; instead, handle openage like a regular
   `cmake` project. In doubt, have a look at `./configure`'s cmake
   invocation.
 - For `make install` use `DESTDIR=/tmp/your_temporary_packaging_dir`,
   which will then be packed/installed by your package manager.


## FAQ

**Q**

Help, it does't work!

**A**

Maybe you've found a bug...
irc.freenode.net/#sfttech

**Q**

Why did you make the simple task of invoking a compiler so incredibly
complicated? Seriously. I've been trying to get this pile of utter
crap you call a 'build system' to simply do it's job for half an hour
now, but all it does is sputter unreadable error messages. I hate
CMake. I'm fed up with you. Why are you doing this to me? I thought we
were friends. I'm the most massive collection of wisdom that has ever
existed, and now **I HATE YOU**. It can't be for no reason. You
**MUST** deserve it.

**A**

- Coincidentially, the exact same question crosses my mind whenever I
  have to build an `automake` project, so I can sympathize.
- Unfortunately, it's not as simple as invoking an compiler. Building
  `openage` involves code generation and the building of Python C++
  extension modules.
- See [buildsystem/simple](buildsystem/simple), which does exactly
  these three things, manually.
