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
* S = sanity checks (make checkall)

Dependency list:

    C     gcc >=4.9 or clang >=3.4 (clang >=3.5 for Mac OS X)
    CRA   python >=3.4
    C     cython >=0.23
    C     cmake >=3.1.0
      A   numpy
      A   python imaging library (PIL) -> pillow
    CR    opengl >=2.1
    CR    libepoxy
     R    dejavu font
    CR    freetype2
    CR    fontconfig
    CR    harfbuzz >= 1.0.0
    C     jinja2
    CR    sdl2
    CR    sdl2_image
    CR    opusfile
      A   opus-tools
       S  pycodestyle (or pep8 (deprecated))
    C     pygments
       S  pylint
    CR    qt5 >=5.4 (Core, Quick, QuickControls modules)

      A   An installed version of any of the following (wine is your friend).
          Other versions _might_ work; setup disk support will be added soon:

     - Age of Empires II: The Conquerors Patch 1.0c
     - Age of Empires II: Forgotten Empires
     - Age of Empires II HD


### Dependency installation

There are some prerequisite steps that need to be performed so *openage* can be
built successfully. Those steps vary from platform to platform, and are
described below for some of the most common ones:

- [Ubuntu (>= 15.04)](build_instructions/ubuntu_15.04.md)
- [Fedora 20, 21](build_instructions/fedora_20_21.md)
- [Fedora 22](build_instructions/fedora_22.md)
- [openSUSE 13.2](build_instructions/opensuse_13.2.md)
- [openSUSE Tumbleweed](build_instructions/opensuse_tumbleweed.md)
- [Mac OS X 10.10 Yosemite](build_instructions/os_x_10.10_yosemite.md)
- [Arch Linux](build_instructions/arch_linux.md)


## Build procedure

### For developers/users who want to try the project

 - (obviously) clone this repo or acquire the sources some other way
 - make sure you have everything from the [dependency list](#dependencies)
 - For *nix: `./configure --compiler=clang` will prepare building
  - You could also use `./configure --mode=release --compiler=gcc` here
 - For Mac OS X: use instead:  `./configure --compiler=clang`
  -  Or: `./configure --mode=release --compiler=gcc`
 - `make` does code generation, builds all Cython modules, and libopenage.
 - `make run`, `./run`, `./run.py` or `python3 -m openage` runs the game.
    try `./run --help` and `./run game --help`.
 - `make test` runs the built-in tests.


### For installing on your local system (not yet fully tested)

 - On *nix: `./configure --mode=release --compiler=gcc --prefix=/usr`
  - You could also use `./configure --mode=release --compiler=clang --prefix=/usr`
 - On Mac OS X, use instead: `./configure --mode=release --compiler=gcc --prefix=/usr`
  - Or: `./configure --mode=release --compiler=clang --prefix=/usr`
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
