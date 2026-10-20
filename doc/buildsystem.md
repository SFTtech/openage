Concept
=======

Our buildsystem is based on `cmake`.
We use a bunch of custom cmake modules and python scripts
to detect libraries and generate code.

The entry point of the build system is `/CMakeLists.txt`.
From there, all buildsystem code of `buildsystem/` is then used.

There is a `/configure` script that wraps the cmake invocation for convenience.

The `/justfile` forwards recipes like `just build` and `just tests` to the build directory.


Components
==========

The buildsystem is pretty sophisticated because *openage* consists of C++
code, generated C++ code, generated Cython code and Python code. All C++
parts are packed in `libopenage`, the Python stuff is in the `openage`
python package. We generate code with the `openage.codegen` Python
package.


Procedure
=========

Steps in building openage:

 - generate `openage/config.py` and `libopenage/config.{h, cpp}` from their `*.in` template files, which contain install prefix and version info
 - run `openage.codegen` (Python module) to generate C++ source files (recipe: `codegen`)
 - generate `.pxd` Cython extension declaration files from annotated `.h` files (recipe: `pxdgen`)
 - build and link `libopenage.so` (recipe: `openage`)
 - build Cython extension modules (generate cpp files and compile them, via `buildsystem.cythonize`) (recipe: `cython`); those link against libopenage.

Additional recipes:

 - see `just --list`
 - `install`
 - `doc` (generate docs via Doxygen)
 - `test` (runs the various tests)
 - various cleaning recipes: `cleanelf`, `cleancodegen`, `cleancython`, `cleaninsourcebuild`, `cleanpxdgen`, `cleanbuilddirs`, `mrproper`, `mrproperer`
 - various compliance checkers: `checkfast`, `checkmerge`, `checkall`, ...


Phases
======

CMake-time: `./configure`
------------------------

cmake reads and interprets all cmake modules and `CMakeLists.txt` files, populating the build directory with build files and generating `config.py`, `config.h` and `config.cpp`. In addition, the codegen script is invoked to determine the list of files it will generate, and the list of (python) files it depends on.

The `./configure` cmake-wrapper script may be used to achieve a pleasant build experience. `./configure` automatically creates the build directory and symlinks it to `bin`, which makes the root `justfile` work.

For each compiler invocation (gcc -Og, clang -O3, etc...), `./configure` creates its own build directory. This allows you to quickly switch compilers and flags (e.g. via `./configure -c clang -O2`) without having to re-build all object files when switching back.


Build time: `ninja`
------------------

`cmake` supports many backends: Project files for various IDEs and more.

By default, `ninja` interprets the generated `build.ninja` at build time; C++ modules need its dependency scanning. It will detect changes in the relevant `cmake` source files, and re-trigger a cmake execution if necessary (just your standard CMake features; pretty cool stuff).

The recipes `codegen`, `libopenage`, `pxdgen`, `cythonize`, `compilepy` and `inplacemodules` are then run as needed.

The `justfile` in the project root directory may be used to invoke `ninja` in the build directory (`ninja -C bin/` would be the manual way of doing this).


Install time `just install`
--------------------------

At install time, the openage library, python modules, Cython extension
modules and game assets are installed to the prefix that was set at cmake
time (default: `/usr/local`), or, if the `DESTDIR` variable is present,
to `$DESTDIR/$INSTALLPREFIX`. Note: The Python/Cython modules have their
separate installation prefix, dictated by the Python environment, and
manually settable via the `--py-prefix` option to `./configure`.

Some more `CMake` magic: the `-rpath` is automagically removed from the
installed binaries (if you don't know what that is: Lucky you. CMake
makes sure you don't need to).


Core buildsystem modules
========================

cpp
---

The [cpp cmake module](/buildsystem/cpp.cmake), apart from verifying the compiler version and setting flags, provides functions for assembling binaries step-by-step.

 - `declare_binary` begins the assembly of a new binary.
 - `add_sources` adds translation units (cpp files) to the binary). The `GENERATED` keyword is required for adding `codegen`-generated translation units, the `MODULE` keyword for C++ module interface units.
 - `finalize_binary` prints information about the binary, and calls the `add_executable` or `add_library` cmake function, depending on its second argument. No more sources may be added after this point. If any generated sources were added to the executable, the `codegen` recipe is added as a `make` dependency.

At build time, this will cause all relevant object files and binaries to be built/linked. At install time, it will install the libs and binaries to /bin and /lib, respectively.

Plain invocations of `add_executable` require all translation units to be specified at once; this system is designed to allow each subfolder to have its own `CMakeLists.txt` file, adding all sources specified in that folder to the main binary.

### C++ modules

New C++ code is written as modules, see the [code style example](/doc/code_style/cat.cppm).
The module name follows the file path: `libopenage/util/color.cpp` is `openage.util.color`.

Usually a module is a single `.cpp` file that holds both interface and implementation:

```cpp
module;

#include <string>

export module openage.dir.name;

import openage.util.color;

export namespace openage::dir {

std::string greet() {
	return "tachauch";
}

} // namespace openage::dir
```

Register it in the local `CMakeLists.txt` after the `MODULE` keyword:

```cmake
add_sources(libopenage MODULE
	name.cpp
)
```

Several modules in one directory are listed together, one file per module:

```cmake
add_sources(libopenage MODULE
    color.cpp
    path.cpp
)
```

Every change to an interface unit recompiles all of its importers.
To avoid that for big implementations, or to keep heavy includes away from importers, split the module into units.
Where a unit is registered depends on its module declaration:

| Unit | Declaration | Registered |
|---|---|---|
| primary interface | `export module openage.dir.name;` | after `MODULE` |
| interface partition | `export module openage.dir.name:part;` | after `MODULE` |
| implementation partition | `module openage.dir.name:part;` | after `MODULE` |
| implementation unit | `module openage.dir.name;` | before `MODULE`, CMake rejects it in the module file set |

A module has exactly one primary interface, its `.cppm` file.
The primary interface must re-export every interface partition with `export import :part;`.
Partitions are only visible inside their module, `import :part;` there; importers only see `openage.dir.name`.
Example of a module whose interface is spread over two files, with the implementation in a third:

```cmake
add_sources(libopenage
    # implementation unit: module openage.dir.name;
    name.cpp
    MODULE
    # primary interface: export module openage.dir.name; export import :part;
    name.cppm
    # interface partition: export module openage.dir.name:part;
    name-part.cppm
)
```

Rules for module units:

 - `#include`s go only in the global module fragment between `module;` and the module declaration; an include after it would attach the header to the module
 - `import`s follow the module declaration
 - `main()` can't be part of a named module

ninja scans the sources for module dependencies, so the build order between modules needs no manual setup.

#### Imports and includes

Headers and `.cpp` files may `import` modules too, which lets legacy code use converted modules and nyan.
GCC, unlike clang, rejects a textual definition after an import that brought the same entity ("redefinition of ..."), hence:

 - `import`s go after the `#include`s of each file (the `cppstyle` check enforces it)
 - never include headers of a library after importing a module that exposes them

Use `<nyan/config.h>` for nyan's type aliases, e.g. `nyan::fqon_t`, and forward declarations when complete classes aren't needed.
Headers that `import nyan;` also include `<nyan/config.h>` before the import so later includes of the config header cannot redefine its constants on GCC.
Use the module for the remaining nyan API, not its other headers.

Standard headers are exempt:
GCC builds compile `<bits/stdc++.h>` into a header unit ([StdHeaderUnit.cmake](/buildsystem/StdHeaderUnit.cmake)), and GCC translates every standard `#include` into an import of it.
CMake can't build header units, so a custom command puts it where GCC looks for them, below the build directory: `bin/usr/.../bits/stdc++.h.gcm`.
With `-flang-info-include-translate`, GCC reports each translated include.

 - importers get the header unit's macros: set macros that change the standard library (`NDEBUG`, `_GLIBCXX_ASSERTIONS`, ...) in the global flags, not per target
 - C++ targets must depend on the header unit with `use_std_header_unit()`; `declare_binary` and `add_cython_modules` do that

python
------

The [python module](/buildsystem/python.cmake) is similar to the cpp module. It checks the Python interpreter and provides functions to declare Cython and Python modules.

 - `add_cython_modules` adds Cython modules (i.e. `.pyx` or `.py` files) that will be compiled. See the code for option docs.
 - `pxdgen` adds C++ headers for `.pxd` file generation (see [pyinterface](/doc/code/pyinterface.md)).
 - `add_pxds` adds any additional `.pxd` or `.pxi` files.
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

read the source, or nag somebody to write more docs on them! wheeee!
