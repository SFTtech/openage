# Building the project

This file should assist you in compiling and running the game.

Note the [troubleshooting](#troubleshooting) and [FAQ](#faq) sections.


## Buildsystem Design

*openage* consists of a pure C++ library, `libopenage`, and the `openage` python package.
There's a special python package, `openage.codegen`, which generates parts of the C++ binary code.

We use `CMake` for all our building needs. The `configure` is a an optional wrapper that will
create a build directory, `bin`, and invoke cmake with the appropriate flags to build inside that directory.
Likewise, the root `Makefile` provides some convenience options that may otherwise be accessed directly in the CMake build folder.

For more build system internals, see [doc/buildsystem.md](/doc/buildsystem.md).

## Dependencies

Dependencies are needed for:

* C = compiling
* R = running
* A = asset conversion

Dependency list:

    C     gcc >=4.9 or clang >=3.4 (clang >=3.5 for Mac OS X)
    CRA   python >=3.4
    C     cython >=0.23
    C     cmake >=3.0.0
      A   numpy
      A   python imaging library (PIL) -> pillow
    CR    opengl >=2.1
    CR    libepoxy
     R    dejavu font
    CR    freetype2
    CR    fontconfig
    CR    harfbuzz >= 1.0.0
    CR    sdl2
    CR    sdl2_image
    CR    opusfile
      A   opus-tools
    C     pygments
    CR    qt5 >=5.4 (Core, Quick, QuickControls, GraphicalEffects modules)

      A   An installed version of any of the following (wine is your friend).
          Other versions _might_ work; setup disk support will be added soon:

     - Age of Empires II: The Conquerors Patch 1.0c
     - Age of Empires II: Forgotten Empires
     - Age of Empires II HD


### Prerequisite steps for Ubuntu users (Ubuntu 15.04)

 - `sudo apt-get update`
 - `sudo apt-get install cmake libfreetype6-dev python3-dev libepoxy-dev libsdl2-dev libsdl2-image-dev libopusfile-dev libfontconfig1-dev libharfbuzz-dev opus-tools python3-pil python3-numpy python3-pygments python3-pip qtdeclarative5-dev qml-module-qtquick-controls qml-module-qtgraphicaleffects`
 - `sudo pip3 install cython`


### Prerequisite steps for Fedora users (Fedora 20, 21)

`sudo yum install cmake gcc-c++ clang SDL2-devel SDL2_image-devel python3-devel python3-numpy python3-pillow libepoxy-devel opus-tools opusfile-devel fontconfig-devel harfbuzz-devel qt5-qtdeclarative-devel qt5-qtquickcontrols qt5-graphicaleffects`


### Prerequisite steps for Fedora users (Fedora 22)

`sudo dnf install cmake gcc-c++ clang SDL2-devel SDL2_image-devel python3-Cython python3-devel python3-numpy python3-pillow python3-pygments libepoxy-devel opus-tools opusfile-devel fontconfig-devel harfbuzz-devel qt5-qtdeclarative-devel qt5-qtquickcontrols qt5-graphicaleffects`


### Prerequisite steps for openSUSE users (openSUSE 13.2)

If you already have python3 or one of packman/games repositories then no
need to run the first two lines. If unsure then just run last line and see
if all packages can be installed.

- `zypper addrepo http://download.opensuse.org/repositories/devel:languages:python3/openSUSE_13.2/devel:languages:python3.repo`
- `zypper refresh`
- `zypper install --no-recommends cmake doxygen fontconfig-devel harfbuzz-devel gcc49-c++ graphviz libSDL2-devel libSDL2_image-devel libfreetype6 libepoxy-devel libogg-devel libopus-devel opusfile-devel pkgconfig python3-Cython python3-Pillow python3-Pygments python3-devel libqt5-qtdeclarative-devel libqt5-qtquickcontrols libqt5-graphicaleffects`


### Prerequisite steps for openSUSE users (openSUSE Tumbleweed)

 - `zypper install --no-recommends cmake doxygen fontconfig-devel harfbuzz-devel gcc-c++ graphviz libSDL2-devel libSDL2_image-devel libfreetype6 libepoxy-devel libogg-devel libopus-devel opusfile-devel pkgconfig python3-Cython python3-Pillow python3-Pygments python3-devel libqt5-qtdeclarative-devel libqt5-qtquickcontrols libqt5-graphicaleffects`


### Prerequisite steps for Mac OS X users (OS X 10.10 Yosemite)

 - Install [Homebrew](http://brew.sh). Other package managers and building from source might also work, but aren't supported as of now. If you figure out fixes for those other options, you're welcome to open pullrequests, as long as the Homebrew build doesn't break.
 - `brew update`
 - `brew install caskroom/cask/brew-cask`
 - `brew tap caskroom/fonts`
 - `brew tap homebrew/versions`
 - `brew tap homebrew/python`
 - `brew update` (yes, again)
 - `brew cask install font-dejavu-sans`
 - `brew install python3 libepoxy freetype fontconfig harfbuzz cmake sdl2 sdl2_image opus opus-tools opusfile`
 - `brew install numpy --with-python3`
 - `brew install pillow --with-python3`
 - `brew install qt5`
 - Choose and Install a compatible compiler:
  - For clang 3.5: `brew install llvm35`
  - For gcc 4.9: `brew install gcc49`
 - Permanently add `/usr/local/bin` to your `$PATH` environment variable
 - Permanently add `/usr/local/lib/pkgconfig` and `/usr/local/lib` to your `$PKG_CONFIG_PATH` environment variable
 - Then run `pip3.5 install pygments cython`


### Prerequisite steps for Arch Linux users

This command should provide required packages for Arch Linux installation:

`sudo pacman -S --needed python python-pillow python-numpy python-pygments cython libepoxy ttf-dejavu freetype2 fontconfig harfbuzz cmake sdl2 sdl2_image opusfile opus-tools python-pylint qt5-declarative qt5-quickcontrols qt5-graphicaleffects`

If you don't have a compiler installed, you can select between these commands to install it:
 - `sudo pacman -S --needed gcc`
 - `sudo pacman -S --needed clang`

You can install both compilers and select the one to be used by `./configure`.


## Build procedure

### For developers/users who want to try the project

 - (obviously) clone this repo or acquire the sources some other way
 - make sure you have everything from the [dependency list](#dependencies)
 - For *nix: `./configure --compiler=clang` will prepare building
  - You could also use `./configure --mode=release --compiler=gcc` here
 - For Mac OS X: use instead:  `./configure --cpp-compiler=clang++-3.5`
  -  Or: `./configure --mode=release --cpp-compiler=g++-4.9`
 - `make` does code generation, builds all Cython modules, and libopenage.
 - `make run`, `./run`, `./run.py` or `python3 -m openage` runs the game.
    try `./run --help` and `./run game --help`.
 - `make test` runs the built-in tests.


### For installing on your local system (not yet fully tested)

 - On *nix: `./configure --mode=release --compiler=gcc --prefix=/usr`
  - You could also use `./configure --mode=release --compiler=clang --prefix=/usr`
 - On Mac OS X, use instead: `./configure --mode=release --cpp-compiler=g++-4.9 --prefix=/usr`
  - Or: `./configure --mode=release --cpp-compiler=clang++-3.5 --prefix=/usr`
 - `make` to compile the project
 - `make install` will install the binary to /usr/bin/openage, python
   packages to `/usr/lib/python...`, static assets to `/usr/share`
   etc. Python packages may use their own prefix, which will be printed.


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

Help, it doesn't work!

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

- Coincidentally, the exact same question crosses my mind whenever I
  have to build an `automake` project, so I can sympathize.
- Unfortunately, it's not as simple as invoking a compiler. Building
  `openage` involves code generation and the building of Cython
  extension modules.
- See [buildsystem/simple](/buildsystem/simple), which does exactly
  these things, manually (don't use this for production).
