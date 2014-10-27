Building openage consists of

 - generating `py/openage/config.py` and `cpp/config.h`, which contain install prefix and version info
 - running the `openage.codegen` python module to generate C++ source files (recipe: `codegen`)
 - building python C++ extension modules (recipe: `pymodules`)
 - building and linking the openage executable (recipe: `openage`)

In addition, the buildsystem allows

 - installing (recipe `install`)
 - generating docs with Doxygen (recipe `doc`)
 - cleaning (recipe `cleanelf`, `cleancodegen`, `cleanpymodules`, `cleaninsourcebuild`, `cleanbuilddirs`, `mrproper`, `mrproperer`; see `make help`)
 - testing using `ctest` (recipe `test`)

At its core, the buildsystem uses `cmake`; the root `CMakeLists.txt` includes modules from the `buildsystem` directory; see in-code documentation for details on individual functions.

Phases
======

cmake-time
----------

cmake reads and interprets  all cmake modules and `CMakeLists.txt` files, populating the build directory with Makefiles and generating the `config.py` and `config.h` files. In addition, the codegen script is invoked to determine the list of files it will generate, and the list of (python) files it depends on.

The `./configure` script may be used to automatically create a build directory, symlink it to `bin`, symlink the to-be-created openage binary to `openage`, and invoke `cmake` with the correct parameters. See `./configure --help` for details.

For each compiler invocation (gcc -Og, clang -O3, etc...), `./configure` creates its own build directory. This allows you to quickly switch compilers and flags (e.g. via `./configure -c clang -O2`) without having to re-build the entire project.

build time
----------

`GNU make` is used to execute the cmake-generated Makefiles at build time. It will detect changes in the relevant `cmake` source files, and re-trigger a cmake execution if necessary.

It then executes the three recipes, `codegen`, `pymodules` and `openage`.

The `Makefile` in the project root directory may be used to invoke `GNU make` in the build directory (`make -C bin/` would be the manual way of doing this).

install time
------------

At install time, the linked openage binary, python modules and built python extension modules and game assets are installed to the prefix that was set at cmake time (default: `/usr/local`), or, if the `DESTDIR` variable is present, to `$DESTDIR/$INSTALLPREFIX`.

Core modules
============

cpp
---

The cpp module in `buildsystem/cpp.cmake`, apart from verifying the compiler version, provides functions for assembling binaries step-by-step.

 - `declare_executable` begins the assembly of a new binary.
 - `add_sources` adds translation units (cpp files) to the binary). The `GENERATED` keyword is required for adding `codegen`-generated translation units.
 - `finalize_executable` prints information about the executable, and calls the `add_executable` cmake function. no more sources may be added after this point. If any generated sources were added to the executable, the `codegen` recipe is added as a `make` dependency.

At build time, this will cause all relevant object files and binaries to be built/linked. At install time, it will install the libs and binaries to /bin and /lib, respectively.

Plain invocations of `add_executable` require all translation units to be specified at once; this system is designed to allow each subfolder to have its own `CMakeLists.txt` file, adding all sources specified in that folder to the main binary.

python
------

The python module in `buildsystem/python.cmake` is similar to the cpp module. It checks for the python version and provides functions to assemble the `setup.py` file. That's right, all this does is offload the work to `distutils`.

 - `add_py_package` adds a python package (i.e. all .py files in a directory) by its fully qualified name (e.g. `add_py_package(openage.convert)`)
 - `add_pyext_module` adds a python C++ extension module (e.g. `add_pyext_module(openage.convert.cabextract.lzxd lzxd.cpp util.cpp)`)
 - `process_python_modules` generates the `setup.py` file from the `buildsystem/templates/setup.py.in` template.

codegen
-------

Provides the function `codegen_run`, which

 - at cmake-time, runs the openage.codegen python module to obtain a list of sources it will generate
 - at build time, actually generates the sources and writes them to the source directory where necessary

It sets the variable `CODEGEN_TARGET_TUS`, containing a list of created `cpp` files. That variable is designed to be used in a call to `add_sources(openage GENERATED ${CODEGEN_TARGET_TUS})`.

The python script automatically determines the python dependencies of the codegen script (via `sys.modules`) and adds them as dependencies of the `codegen` recipe. When the list of python dependencies, or output TUS, have changed, it automatically triggers a new `cmake` run.

other modules
-------------

read the source, or nag somebody to write more docs on them! wheeee!
