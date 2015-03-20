Steps in building openage:

 - generate `py/openage/config.py` and `cpp/config.h`, which contain install prefix and version info
 - run `openage.codegen` (python module) to generate C++ source files (recipe: `codegen`)
 - generate `.pxd` cython extension declaration files from annotated `.h` files (recipe: `pxdgen`)
 - build and link `libopenage.so` (recipe: `openage`)
 - build Cython extension modules (generate cpp files and compile them, via `distutils.cythonize`) (recipe: `python`); those link against libopenage.

Additional recipes:

 - `install`
 - `doc` (generate docs via Doxygen)
 - `test` (testing using CMake's `ctest`)
 - various cleaning recipes: `cleanelf`, `cleancodegen`, `cleanpython`, `cleaninsourcebuild`, `cleanpxdgen`, `cleanbuilddirs`, `mrproper`, `mrproperer`; see `make help`
 - various compliance checkers: `check`, `checkfull`, `pylint`, ...; see `make help`

At its core, the buildsystem uses `cmake`; the root `CMakeLists.txt` includes modules from the `buildsystem` directory; see in-code documentation for details on individual functions.

Phases
======

cmake-time
----------

cmake reads and interprets all cmake modules and `CMakeLists.txt` files, populating the build directory with Makefiles and generating the `config.py` and `config.h` files. In addition, the codegen script is invoked to determine the list of files it will generate, and the list of (python) files it depends on.

The `./configure` cmake-wrapper script may be used to achieve a pleasant build experience. `./configure` automatically creates the build directory and symlinks it to `bin`, which makes the root Makefile work.

For each compiler invocation (gcc -Og, clang -O3, etc...), `./configure` creates its own build directory. This allows you to quickly switch compilers and flags (e.g. via `./configure -c clang -O2`) without having to re-build all object files when switching back.

build time
----------

`GNU make` is used to interpret the cmake-generated Makefiles at build time. It will detect changes in the relevant `cmake` source files, and re-trigger a cmake execution if necessary (just your standard CMake features; pretty awesome).

The recipes `codegen`, `openage`, `pxdgen` and `python` are then executed as needed.

The `Makefile` in the project root directory may be used to invoke `GNU make` in the build directory (`make -C bin/` would be the manual way of doing this).

install time
------------

At install time, the linked openage binary, python modules and python extension modules and game assets are installed to the prefix that was set at cmake time (default: `/usr/local`), or, if the `DESTDIR` variable is present, to `$DESTDIR/$INSTALLPREFIX`.

cmake automatically removes the `-rpath` from the shared object, and `distutils` re-builds the extension modules, also skipping the `-rpath` in the process.

Core modules
============

cpp
---

The cpp module in `buildsystem/cpp.cmake`, apart from verifying the compiler version, provides functions for assembling binaries step-by-step.

 - `declare_binary` begins the assembly of a new binary.
 - `add_sources` adds translation units (cpp files) to the binary). The `GENERATED` keyword is required for adding `codegen`-generated translation units.
 - `finalize_binary` prints information about the binary, and calls the `add_executable` or `add_library` cmake function, depending on its second argument. No more sources may be added after this point. If any generated sources were added to the executable, the `codegen` recipe is added as a `make` dependency.

At build time, this will cause all relevant object files and binaries to be built/linked. At install time, it will install the libs and binaries to /bin and /lib, respectively.

Plain invocations of `add_executable` require all translation units to be specified at once; this system is designed to allow each subfolder to have its own `CMakeLists.txt` file, adding all sources specified in that folder to the main binary.

python
------

The python module in `buildsystem/python.cmake` is similar to the cpp module. It checks for the python version and provides functions to assemble the `setup.py` file. That's right, all this does is offload the work to `distutils`.

 - `add_py_packages` adds python packages (i.e. all .py files in a directory) by its fully-qualified name (e.g. `add_py_package(openage.convert)`)
 - `add_cython_modules` adds cython extension modules by their filenames; in addition to `.pyx` files, regular `.py` files may be specified as well.
 - `pxdgen` adds C++ `.h` files as sources for the `.pxd` file generation
 - `python_finalize` generates the config file for the `distutils` `setup.py` script, as well as the lists created by the above three functions, in the `bin/py` folder.

As part of the `pxdgen` build target, `buildsystem/pxdgen.py` is run, placing the generated `.pxd` files in a directory `bin/py/pxds/libopenage`; the structure of that directory mirrors the structure of `cpp/`.

codegen
-------

Provides the function `codegen_run`, which

 - at cmake-time, runs the openage.codegen python module to obtain a list of sources it will generate
 - at build time, actually generates the sources and writes them to the source directory where necessary

It sets the variable `CODEGEN_TARGET_TUS`, containing a list of created `cpp` files. That variable is designed to be used in a call to `add_sources(openage GENERATED ${CODEGEN_TARGET_TUS})`.

The python script automatically determines the python dependencies of the codegen script (via `sys.modules`) and adds them as dependencies of the `codegen` recipe. When the list of python dependencies, or output TUS, have changed, it automatically triggers a new `cmake` run.

The generated `.cpp` files are placed in the `cpp` folder, but all end in `.gen.cpp`.

other modules
-------------

read the source, or nag somebody to write more docs on them! wheeee!
