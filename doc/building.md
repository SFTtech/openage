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

    C     gcc >=10 or clang >=10
    CRA   python >=3.9
    C     cython >=3.0.10 OR (>=0.29.31 AND <=3.0.7)
    C     cmake >=3.16
      A   numpy
      A   lz4
      A   python imaging library (PIL) -> pillow
     RA   setuptools (for python>=3.12 and cython<3.1)
      A   toml
    CR    opengl >=3.3
    CR    libepoxy
    CR    libpng
       S  clang-tidy
     R    dejavu font
    CR    eigen >=3
    CR    freetype2
    CR    fontconfig
    CR    harfbuzz
    C   O include-what-you-use
    CR    nyan  (https://github.com/SFTtech/nyan)
    CR  O ncurses
    C     mako
    CR    opusfile
    CRA   opus
    CRA   ogg
       S  pycodestyle
    C     pygments
       S  pylint
    CR    qt6 >=6.2 (Core, Quick, QuickControls, Multimedia modules)
    CR    toml11
    CR  O vulkan

      A   An installed version of any of the following (wine is your friend).
          Other versions _might_ work:

     - Age of Empires I: Rise of Rome Patch 1.0a
     - Age of Empires I: Definitive Edition
     - Age of Empires II: The Conquerors Patch 1.0c
     - Age of Empires II: Forgotten Empires
     - Age of Empires II: HD (now also called: Age of Empires II (2013))
     - Age of Empires II: Definitive Edition
     - Star Wars: Galactic Battlegrounds: Clone Campaigns


### Dependency installation

There are some prerequisite steps that need to be performed so *openage* can be
built successfully. Those steps vary from platform to platform, and are
described below for some of the most common ones:

- [Ubuntu](build_instructions/ubuntu.md)
- [Debian](build_instructions/debian.md)
- [Fedora](build_instructions/fedora.md)
- [openSUSE](build_instructions/opensuse.md)
- [macOS](build_instructions/macos.md)
- [Arch Linux](build_instructions/arch_linux.md)
- [FreeBSD](build_instructions/freebsd.md)
- [Gentoo](build_instructions/gentoo.md)
- [Nix/NixOS](build_instructions/nix.md)
- [Microsoft Windows](build_instructions/windows_msvc.md)

### nyan installation

`openage` depends on [`nyan`](https://github.com/SFTtech/nyan), which is the
engine configuration language.


* For development, `nyan` can be built and used **without installation**
(-> no "`make install`", since it can be found using [`cmake` user package registry](https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#user-package-registry)).
Just clone the repo somewhere and [follow the `nyan` build instructions](https://github.com/SFTtech/nyan/blob/master/doc/building.md)).

* Alternatively, `openage` can download `nyan` automatically. This is
  activated with `./configure --download-nyan ...` or `cmake
  -DDOWNLOAD_NYAN=YES ...`.

`cmake` looks for `nyan` in the user package registry (`~/.cmake/packages/nyan/`)
and the system package registry (`/usr/lib/cmake/nyan/`) for `nyanConfig.cmake`.

If `nyan` cannot be found but you know where it is, you can hint the location of the `nyanConfig.cmake` with:
```
-Dnyan_DIR=/directory/where/nyanConfig/is/in/
```


## Build procedure

Make sure you have all the dependencies installed.

### Development

- (Obviously) clone this repo or acquire the sources some other way
- Make sure you have everything from the [dependency list](#dependencies)
- Select the compiler and mode: see `./configure --help`
  - Linux etc: `./configure`
  - macOS:  `./configure --compiler=clang`
    - hassle-free building on macOS (if above is not working)
      - get the latest llvm build from: `https://github.com/llvm/llvm-project/releases/latest`
        - (yeah, xcode still needs to be installed)
      - install python deps: see [pip(3) install](https://github.com/SFTtech/openage/tree/master/doc/build_instructions/macos.md)
      - install build deps (without llvm): see [brew install](https://github.com/SFTtech/openage/tree/master/doc/build_instructions/macos.md)
      - configure: `CC=/path/to/downloaded-llvm/bin/clang CXX=/path/to/downloaded-llvm/bin/clang++ LD=/path/to/downloaded-llvm/bin/ld64.lld ./configure --download-nyan`
      - a small build hint for later: use your freaking cores with the -j option later `make -j$(sysctl -n hw.ncpu)`
- `make` generates and builds everything
- `make run` or `bin/run` launches the game. Try `bin/run --help`!
- `make test` runs the built-in tests.


### Release

Disclaimer: Use your distribution package of `openage` instead!
Your distro package maintainers do all the nasty work for you,
and will provide you with updates!
Also, you don't need to `make install`, you can run `openage` within its git repo.

 - Set build mode: `./configure --mode=release --compiler=clang --prefix=/usr/local`
 - `make`
 - `make install` install the game to `/usr/local`
   - beware, this will add *untracked* files to your drive
   - please use your distribution package instead!
 - launch `openage`, it's in `/usr/local/bin/openage`


### For packagers

The reference package is [created for Gentoo](https://github.com/SFTtech/gentoo-overlay/blob/master/games-strategy/openage/).

- Don't use `./configure`; instead, handle openage like a regular
  `cmake` project. In doubt, have a look at `./configure`'s cmake
  invocation.
- To specify the to-be-used python version (or rather, executable),
  pass `-DPython3_EXECUTABLE=...` or `-DPython3_ROOT_DIR=...` to `cmake`
- Use `make install DESTDIR=/tmp/your_temporary_packaging_dir`,
  which will then be packed/installed by your package manager.

### Troubleshooting

- I wanna see compiler invocations
  - `make VERBOSE=1`
- My `Qt`/`Python`/whatever is installed somewhere, but `cmake` can't find it!
  - Run `ccmake` or `cmake-gui` in the build directory to see and change config variables.
  - You can manually tell `cmake` where to look. Try something along the lines of
    - `./configure -- -DPYTHON_INCLUDE_DIRS=/whereever/python/include/`
    - `-DPython3_EXECUTABLE=/your/py3/directory/`

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

* Why don't you `$proposition`? Your `$component` is crap
  and would be much better then!
  - Cool, we totally missed that! [Tell us more](/README.md#contact) and [submit your ideas!](/doc/contributing.md)
