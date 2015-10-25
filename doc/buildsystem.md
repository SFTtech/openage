Steps in building openage:

 - generate `py/openage/config.py` and `libopenage/config.{h, cpp}`, which contain install prefix and version info
 - run `openage.codegen` (python module) to generate C++ source files (recipe: `codegen`)
 - generate `.pxd` cython extension declaration files from annotated `.h` files (recipe: `pxdgen`)
 - build and link `libopenage.so` (recipe: `openage`)
 - build Cython extension modules (generate cpp files and compile them, via `buildsystem.cythonize`) (recipe: `cython`); those link against libopenage.

Additional recipes:

 - see `make help`
 - `install`
 - `doc` (generate docs via Doxygen)
 - `test` (runs the various tests)
 - various cleaning recipes: `cleanelf`, `cleancodegen`, `cleancython`, `cleaninsourcebuild`, `cleanpxdgen`, `cleanbuilddirs`, `mrproper`, `mrproperer`
 - various compliance checkers: `checkfast`, `checkall`, ...

At its core, the buildsystem uses `cmake`; the root `CMakeLists.txt` includes modules from the `buildsystem` directory; see in-code documentation for details on individual functions.

Phases
======

CMake-time ("./configure")
--------------------------

cmake reads and interprets all cmake modules and `CMakeLists.txt` files, populating the build directory with Makefiles and generating `config.py`, `config.h` and `config.cpp`. In addition, the codegen script is invoked to determine the list of files it will generate, and the list of (python) files it depends on.

The `./configure` cmake-wrapper script may be used to achieve a pleasant build experience. `./configure` automatically creates the build directory and symlinks it to `bin`, which makes the root Makefile work.

For each compiler invocation (gcc -Og, clang -O3, etc...), `./configure` creates its own build directory. This allows you to quickly switch compilers and flags (e.g. via `./configure -c clang -O2`) without having to re-build all object files when switching back.

Build time ("make")
-------------------

`GNU make` is used to interpret the cmake-generated Makefiles at build time. It will detect changes in the relevant `cmake` source files, and re-trigger a cmake execution if necessary (just your standard CMake features; pretty cool stuff).

The recipes `codegen`, `libopenage`, `pxdgen`, `cythonize`, `compilepy` and `inplacemodules` are then run as needed.

The `Makefile` in the project root directory may be used to invoke `GNU make` in the build directory (`make -C bin/` would be the manual way of doing this).

Install time ("make install")
-----------------------------

At install time, the linked openage binary, python modules, Cython extension modules and game assets are installed to the prefix that was set at cmake time (default: `/usr/local`), or, if the `DESTDIR` variable is present, to `$DESTDIR/$INSTALLPREFIX`. Note: The (P|C)ython modules have their separate installation prefix, dictated by the Python environemnt, and manually settable via the `--py-prefix` option to `./configure`.

Some more `CMake` magic: the `-rpath` is automagically removed from the installed binaries (if you don't know what that is: Lucky you. CMake makes sure you don't need to).

Core modules
============

cpp
---

The [cpp module](/buildsystem/cpp.cmake), apart from verifying the compiler version and setting flags, provides functions for assembling binaries step-by-step.

 - `declare_binary` begins the assembly of a new binary.
 - `add_sources` adds translation units (cpp files) to the binary). The `GENERATED` keyword is required for adding `codegen`-generated translation units.
 - `finalize_binary` prints information about the binary, and calls the `add_executable` or `add_library` cmake function, depending on its second argument. No more sources may be added after this point. If any generated sources were added to the executable, the `codegen` recipe is added as a `make` dependency.

At build time, this will cause all relevant object files and binaries to be built/linked. At install time, it will install the libs and binaries to /bin and /lib, respectively.

Plain invocations of `add_executable` require all translation units to be specified at once; this system is designed to allow each subfolder to have its own `CMakeLists.txt` file, adding all sources specified in that folder to the main binary.

python
------

The [python module](/buildsystem/python.cmake) is similar to the cpp module. It checks the Python interpreter and provides functions to declare Cython and Python modules.

 - `add_cython_modules` adds Cython modules (i.e. `.pyx` or `.py` files) that will be compiled. See the code for option docs.
 - `pxdgen` adds C++ headers for `.pxd` file generation (see [pyinterface](/doc/code/pyinterface.md)).
 - `add_pxd` adds any additional `.pxd` or `.pxi` files.
 - `add_py_modules` adds pure-Python modules; note that you also must declare `.py` files that were declared in `add_cython_modules`.

codegen
-------

Provides the function `codegen_run`, which

 - at cmake-time, runs the openage.codegen python module to obtain a list of sources it will generate
 - at build time, actually generates the sources and writes them to the source directory where necessary

It sets the variable `CODEGEN_TARGET_TUS`, containing a list of created `cpp` files. That variable is designed to be used in a call to `add_sources(openage GENERATED ${CODEGEN_TARGET_TUS})`.

The python script automatically determines the python dependencies of the codegen script (via `sys.modules`) and adds them as dependencies of the `codegen` recipe. When the list of python dependencies, or output TUS, have changed, it automatically triggers a new `cmake` run.

The generated `.cpp` files are placed in the `libopenage` folder, and all end in `.gen.cpp` (or `.gen.h`).

other modules
-------------

Read the source, or nag somebody to write more docs on them! Wheeee!
