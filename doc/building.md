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
script is a _cmake wrapper_ that will create a build directory and
invoke cmake with the appropriate flags. The `Makefile` in the project
root acts as a wrapper around several useful features.

For more build system internals, see [doc/buildsystem.md](/doc/buildsystem.md).


## Dependencies

Dependencies are needed for:

* C = compiling
* R = running
* M = media convert script

Dependency list:

    CRM   python >=3.3
      M   python imaging library (PIL) -> pillow
      M   numpy
    CR    opengl >=2.1
    CR    libepoxy
    CR    ftgl
     R    dejavu font
    CR    freetype2
    CR    fontconfig
    C     cmake >=2.8.8
    CR    sdl2
    CR    sdl2_image
    CR    opusfile
      M   opus-tools
    C     gcc >=4.9 or clang >=3.4

      M   An installed version of any of the following (wine is your friend).
          Other versions _might_ work; setup disk support will be added soon:

     - Age of Empires II: The Conquerors Patch 1.0c
     - Age of Empires II: Forgotten Empires
     - Age of Empires II HD


### Prerequisite steps for Ubuntu users (Ubuntu 14.10)

 - `sudo apt-get update`
 - `sudo apt-get install cmake libfreetype6-dev python3-dev libepoxy-dev libsdl2-dev ftgl-dev  libsdl2-image-dev libopusfile-dev libfontconfig1-dev opus-tools python3-pil python3-numpy`


### Prerequisite steps for Fedora users (Fedora 20, 21)

`sudo yum install cmake gcc-c++ clang SDL2-devel SDL2_image-devel python3-devel python3-numpy python3-pillow ftgl-devel libepoxy-devel opus-tools opusfile-devel fontconfig-devel`


### Prerequisite steps for openSUSE users (openSUSE 13.1)

If you already have python3 or one of packman/games repositories then no
need to run the first two lines.
If unsure then just run last line and see if all packages can be installed.
Also feel free to drop `--no-recommends` from last line.

 - `zypper addrepo http://download.opensuse.org/repositories/devel:languages:python3/openSUSE_13.1/devel:languages:python3.repo`
 - `zypper addrepo http://packman.inode.at/suse/openSUSE_13.1/ packman`
 - `zypper refresh`
 - `zypper install --no-recommends cmake fontconfig-devel ftgl-devel gcc-c++ libepoxy-devel libfreetype6 libogg-devel libopus-devel libSDL2-devel libSDL2_image-devel opusfile-devel opus-tools python3-devel python3-numpy python3-Pillow`


### Prerequisite steps for Mac OS X users (OS X 10.10 Yosemite)

 - Install [homebrew](http://brew.sh). Other package managers and building from source might also work, but aren't supported as of now. If you figure out fixes for those other options, you're welcome to open pullrequests, as long as the homebrew build doesn't break.
 - Install the needed fonts (e.g. using [homebrew-cask](https://github.com/caskroom/homebrew-cask)).
 - `brew tap homebrew/python`
 - `brew update`
 - `brew install python3 libepoxy ftgl freetype fontconfig cmake sdl2 sdl2_image opus opus-tools opusfile`
 - `brew install numpy --with-python3`
 - `brew install pillow --with-python3`


### Prerequisite steps for Arch Linux users

This command should provide required packages for Arch Linux installation:

`sudo pacman -S --needed python python-pillow python-numpy libepoxy ftgl ttf-dejavu freetype2 fontconfig cmake sdl2 sdl2_image opusfile opus-tools`

If you don't have a compiler installed, you can select between these commands to install it:
 - `sudo pacman -S --needed gcc`
 - `sudo pacman -S --needed clang`

You can install both compilers and select the one to be used by `./configure`.


## Build procedure

### For developers/users who want to try the project

 - (obviously) clone this repo or acquire the sources some other way
 - make sure you have everything from the [dependency list](#dependencies)
 - `./configure --mode=debug --compiler=llvm` will prepare building
  - You could also use `./configure --mode=debug --compiler=gnu` here
 - `make` will do code generation, build all python modules and the
   openage executable
 - `make media AGE2DIR="$HOME/.wine/drive_c/age2"` will convert all media
   files from the given age2 install folder, storing them in
   `./assets/converted`
 - `make run` or `./openage` will run the game. try
   `./openage --help`
 - `make test` will run the unit tests


### For installing on your local system

 - `./configure --mode=release --compiler=gnu --prefix=/usr`
  - You could also use `./configure --mode=release --compiler=llvm --prefix=/usr`
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


### Troubleshooting

 - I wanna see compiler invocations
  - `make VERBOSE=1`
 - My `SDL2_Image`/`PythonInterp`/whatever is installed somewhere, but `cmake` can't find it!
  - You can manually tell `cmake` where to look. Try something along the lines of
    `./configure --raw-cmake-args -DSDL2IMAGE_INCLUDE_DIRS=/whereever/sdl2_image/include/`
 - I get nonsensical cmake errors like `unable to execute /home/user/git/openage/clang++`
  - This is an issue with `cmake`, or rather, your usage of it. You probably invoked `cmake` directly,
    and defined a compiler even though the build directory has already been initialized.
    cmake is a bit "special" in this regard. Simply omit the compiler definitions, or clean the build
    directory.
 - I get compiler errors about missing header files
  - Make sure to install the developer version (including header files) of the library in question.


## FAQ

**Q**

Help, it does't work!

**A**

Have a look at [Troubleshooting](#troubleshooting) above.
Maybe you've found a bug... `irc.freenode.net/#sfttech`

**Q**

Why did you make the simple task of invoking a compiler so incredibly
complicated? Seriously. I've been trying to get this pile of utter
crap you call a 'build system' to simply do its job for half an hour
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
- See [buildsystem/simple](/buildsystem/simple), which does exactly
  these three things, manually.
