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
* O = optional, we can continue without it

Dependency list:

    C     gcc >=7 or clang >=5
    CRA   python >=3.6
    C     cython >=0.25
    C     cmake >=3.16
      A   numpy
     RA   toml
    CR    opengl >=3.3
    CR    libepoxy
    CR    libpng
     R    dejavu font
    CR    eigen >=3
    CR    freetype2
    CR    fontconfig
    CR    harfbuzz >=1.0.0
    CR    nyan  (https://github.com/SFTtech/nyan)
    CR  O ncurses
    C     jinja2
    CR    sdl2
    CR    sdl2_image
    CR    opusfile
    CRA   opus
    CRA   ogg
       S  pycodestyle
    C     pygments
       S  pylint
    CR    qt5 >=5.9 (Core, Quick, QuickControls modules)
    CR  O vulkan

      A   An installed version of any of the following (wine is your friend).
          Other versions _might_ work:

     - Age of Empires II: The Conquerors Patch 1.0c
     - Age of Empires II: Forgotten Empires
     - Age of Empires II HD


### Dependency installation

There are some prerequisite steps that need to be performed so *openage* can be
built successfully. Those steps vary from platform to platform, and are
described below for some of the most common ones:

- [Ubuntu (>= 18.04)](build_instructions/ubuntu.md)
- [Debian Unstable](build_instructions/debian.md)
- [Fedora (>= 27)](build_instructions/fedora.md)
- [openSUSE 13.2](build_instructions/opensuse_13.2.md)
- [openSUSE Tumbleweed](build_instructions/opensuse_tumbleweed.md)
- [Mac OS X 10.14 Mojave](build_instructions/os_x_10.14_mojave.md)
- [Arch Linux](build_instructions/arch_linux.md)
- [FreeBSD](build_instructions/freebsd.md)
- [Gentoo](build_instructions/gentoo.md)
- [Microsoft Windows 10](build_instructions/windows_msvc.md)


### nyan installation

`openage` depends on [`nyan`](https://github.com/SFTtech/nyan), which is the
engine configuration language.


* For development, `nyan` can be built and used **without installation** (->
no "`make install`"). Just clone the repo somewhere and [follow the `nyan`
build instructions](https://github.com/SFTtech/nyan/blob/master/doc/building.md)).

* Alternatively, `openage` can download `nyan` automatically. This is
  activated with `./configure --download-nyan ...` or `cmake
  -DDOWNLOAD_NYAN=YES ...`.

**Only install** `nyan` when packaging it. You don't need to clobber your
filesystem.

`cmake` looks for `nyan` in the user package registry
(`~/.cmake/packages/nyan/`) and the system package registry
(`/usr/lib/cmake/nyan/`) for `nyanConfig.cmake`.

If `nyan` cannot be found but you know where it is, you can hint the
location of the `nyanConfig.cmake` with
`-Dnyan_DIR=/directory/where/nyanConfig/is/in/`.


## Build procedure

Make sure you have all the dependencies installed.

### Development

- (Obviously) clone this repo or acquire the sources some other way
- Make sure you have everything from the [dependency list](#dependencies)
- Select the compiler and mode: see `./configure --help`
  - Linux etc: `./configure`
  - macOS:  `./configure --compiler=clang`, `./configure --mode=release --compiler=gcc`
- `make` generates and builds everything
- `make run` or `bin/run` launches the game. Try `bin/run --help`!
- `make test` runs the built-in tests.


### Release

Disclaimer: Use your distribution package of `openage` instead!
Your distro package maintainers do all the nasty work for you,
and will provide you with updates!

 - Set build mode: `./configure --mode=release --compiler=clang --prefix=/usr/local`
 - `make`
 - `make install` install the game to `/usr/local`
   - beware, this will add *untracked* files to your drive
   - please use your distribution package instead!
 - launch `openage`, it's in `/usr/local/bin/openage`


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
  - Run `ccmake` or `cmake-gui` in the build directory to see and change config variables.
  - You can manually tell `cmake` where to look. Try something along the lines of
    `./configure -- -DSDL2IMAGE_INCLUDE_DIRS=/whereever/sdl2_image/include/`

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
