# Building the project

This file should assist you in compiling and running the game.

Note the [troubleshooting](#troubleshooting) and [FAQ](#faq) sections.


## Buildsystem Design

*openage* consists of a pure C++ library, `libopenage`, and the `openage` python package.

We use `CMake` for all our building needs.
We wrap `CMake` with an optional `configure` wrapper script.
You can build the project like a regular `CMake` project.

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
    CR    sdl2
    CR    sdl2_image
    CR    opusfile
      A   opus-tools
       S  pycodestyle (or pep8 (deprecated))
    C     pygments
       S  pylint
    CR    qt5 >=5.5 (Core, Quick, QuickControls modules)

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

### Development

- (Obviously) clone this repo or acquire the sources some other way
- Make sure you have everything from the [dependency list](#dependencies)
- Select the compiler and mode: see `./configure --help`
  - Linux etc: `./configure`
  - macOS:  `./configure --compiler=clang`, `./configure --mode=release --compiler=gcc`
- `make` generates and builds everything
- `make run` or `./run` launches the game. Try `./run --help`!
- `make test` runs the built-in tests.


### Release

Disclaimer: Use your distribution package of `openage` instead!
Your distro package maintainers do all the nasty work for you,
and will provide you with updates!

 - Set build mode: `./configure --mode=release --compiler=clang --prefix=/usr`
 - `make`
 - `make install` install the game to `/usr`
 - `python3 -m openage` launch!


### For packagers

 - Don't use `./configure`; instead, handle openage like a regular
   `cmake` project. In doubt, have a look at `./configure`'s cmake
   invocation.
 - Use `make install DESTDIR=/tmp/your_temporary_packaging_dir`,
   which will then be packed/installed by your package manager.


### Troubleshooting

- I wanna see compiler invocations
  - `make VERBOSE=1`
- My `SDL2_Image`/`PythonInterp`/whatever is installed somewhere, but `cmake` can't find it!
  - You can manually tell `cmake` where to look. Try something along the lines of
    `./configure --raw-cmake-args -DSDL2IMAGE_INCLUDE_DIRS=/whereever/sdl2_image/include/`

- I get compiler errors about missing header files
  - Make sure to install the developer version (including header files) of the library in question.
- I get nonsensical cmake errors like `unable to execute /home/user/git/openage/clang++`
  - This is an issue with `cmake`, or rather, your usage of it. You probably invoked `cmake` directly,
    and defined a compiler even though the build directory has already been initialized.
    `cmake` is a bit "special" in this regard. Simply omit the compiler definitions,
    or purge the build directory with `rm -rf build bin .bin`.


## FAQ

* Help, it doesn't work!

  * Have a look at [Troubleshooting](#troubleshooting) above.
    Maybe you've found a bug... [Contact us!](/README.md#contact)

* Why did you make the simple task of invoking a compiler so incredibly
  complicated? Seriously. I've been trying to get this pile of utter
  crap you call a 'build system' to simply do its job for half an hour
  now, but all it does is sputter unreadable error messages. I hate
  CMake. I'm fed up with you. Why are you doing this to me? I thought we
  were friends. I'm the most massive collection of wisdom that has ever
  existed, and now **I HATE YOU**. It can't be for no reason. You
  **MUST** deserve it.

  - Coincidentally, the exact same question crosses my mind whenever I
    have to build an `automake` project, so I can sympathize.
  - Unfortunately, it's not as simple as invoking a compiler. Building
    `openage` involves code generation and the building of Cython
    extension modules.
  - See [buildsystem/simple](/buildsystem/simple), which does exactly
    these things, manually (don't use this for production).

* Why don't you `$proposition`? Your `$component` is crap
  and would be much better then!
  - Cool, we totally missed that! [Tell us more](/README.md#contact) and [submit your ideas!](/doc/contributing.md)
