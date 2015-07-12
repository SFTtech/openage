Openage C++ <-> Python interface
================================

Openage consists of python modules, which contain the program entry point,
and the library `libopenage.so`, which contains all C++ code.

Cython is used for glue code.

Quick overview of Cython
------------------------

Cython modules are written in `.pyx` files, and roughly equivalent to .py files.
In addition to regular Python syntax, `.pyx` files allow you to define typed
functions and objects:

``` cython
cdef int square(int x):
    return x * x

cdef cppclass Rectangle:
    int h, w

    int size():
        return this.h * this.w

def foo():
    Rectangle r
    r.h = 5
    r.w = 6
    return square(r.size())
```

`.pyx` files are translated to `.cpp` by Cython ("cythonized") as part of the
openage build process; syntax errors are shown in this step.
Each `.cpp` file is then compiled to a Python extension module, which may be
used from everywhere.

    - `def` functions and `cdef class` classes can be used from Python.
    - `cdef` functions are suitable for storage in a `C` function pointer.

Cython can use any regular C++ function or type; for that purpose, they
need to be declared in `.pxd` files (which are analogous to C++ `.h` files):

``` cython
cdef extern from "<stdlib.h>":
    int atoi(const char *s)
```

To use this function declared in `foo.pxd`,

``` cython
from foo cimport atoi as c_atoi

def atoi(s):
    return c_atoi(s)
```

Openage has a helper, `pxdgen`, which auto-generates `.pxd` files for `.h`
files in the `libopenage/` subdirectory, from `pxd:` annotations in these files, as part
of the build system.

The pxd annotations are really simple; just have a look at some of the headers.
You'll find the generated `.pxd` files next to the C++ header files.

To `cimport` a class Foo that was pxd-annotated in `util/foo.h`, type

``` cython
from libopenage.util.foo cimport Foo
```

Cython [ships `.pxd` files](https://github.com/cython/cython/tree/master/Cython/Includes) for most C, C++ and CPython functions:

``` cython
from libc.math cimport sin
from libcpp.vector cimport vector

cdef vector[float] vector_sin(vector[float] args):
    return [sin(arg) for arg in args]

print(vector_sin(range(10)))
```

From time to time, it may be useful to have a look at the generated `.cpp` file,
especially if you require more exotic functionality or something doesn't work
out as you expected.


Calling C++ functions from Python
---------------------------------

To make a C++ function available for calling from Cython code, annotate the header
file with `pxd` comments:

`libopenage/example.cpp`

``` cpp
namespace openage {

int foo(int arg0, std::string arg1) {
        return 5;
}

} // openage
```

`libopenage/example.h`

``` cpp
// pxd: from libcpp.string cimport string
#include <string>

namespace openage {

/**
 * The famous foo function. Warning: might bar occasionally.
 *
 * pxd:
 *
 * int foo(int arg0, string arg1) nogil except +
 */
int foo(int arg0, std::string arg1);

} // openage
```

_Always_ pxd-declare your functions as `except +` ("may potentially rise a C++ exception"), unless the C++ function is marked `noexcept` itself.
If a function that is not declared `except +` throws anyways, the entire CPython interpreter is likely to be shredded.

`cmake` must be informed about the `pxd`-annotated header file:

`libopenage/CMakeLists.txt`

``` cmake
pxdgen(example.h)
```

The function is now available from Cython. To make it available for pure-python modules, write a wrapper:

`openage/foo.pyx`

``` cython
from libopenage.foo cimport foo as c_foo

def foo(int arg0, str arg1):
    with nogil:
        return c_foo(arg0, arg1)
```

`openage/bar.py`

``` python
from openage.foo import foo

print(foo(10, "test"))
```


Calling Python functions from C++
---------------------------------

The interface works one-way: Cython can access `libopenage`, but not the other way round.
Thus, `libopenage` must provide `PyIfFunc` function pointers that are filled by Cython
during initialization.

Any `cdef` functions may be stored in C++ function pointers; the `openage::pyinterface::PyIfFunc`
type has been created for this purpose; it allows binding arguments and makes sure that the pointer
is properly initialized (instead of invoking undefined behavior if called before initialization).

We will call this pure-Python function from C++:

`openage/bar.py`

``` python
def bar(arg0, arg1):
    """
    This function involves rainbows and unicorns.

    arg0 shall be an integer, and arg1 a string.
    """
    return 6.283185307179586
```

Declare, define and pxd-export the function pointer:

`libopenage/foo.cpp`

``` cpp
#include "foo.h"

PyIfFunc<float, int, std::string> bar;
```

`libopenage/foo.h`

``` cpp
// pxd: from libcpp.string cimport string
#include <string>

#include "pyinterface/functional.h"

// pxd: PyIfFunc2[float, int, string] bar
extern PyIfFunc<float, int, std::string> bar;
```

Wrap the python function in a `cdef` function, and define a method `setup()`,
which binds the `cdef` function to the PyIfFunc object.

`openage/foo.pyx`

``` cython
from libopenage.foo cimport bar as c_bar

from .bar import bar as py_bar

cdef float bar(int arg0, string arg1) except * with gil:
    return py_bar(arg0, <str> arg1)

def setup():
    c_bar.bind0(bar)
```

`PyIfFunc2` means that the function takes 2 arguments, and `bind0` means that 0
arguments are bound (this is needed because Cython currently doesn't support variadic template arguments).

Add a call to `openage.foo.setup()` to `openage.cppinterface.setup.setup`.

`openage/pyinterface/setup.pyx`

``` python
def setup():
    # (...)

    from openage.foo import setup
    setup()

    # (...)
```

If you forget to do that, `openage.pyinterface.setup.setup` will raise a fatal exception.

`libopenage/bar.cpp`

``` cpp
#include "foo.h"

std::cout << openage::bar(5, "test") << std::endl;
```

The __only__ way of accessing Python code from C++ should be via the `Func` or `PyIfFunc` function wrappers, as those
guarantee that exceptions are properly translated, among other things.


Real-life examples
------------------

For code that wraps a C++ class for Python, see `openage/cabextract/lzxd.pyx` and `libopenage/util/lzxd.h`.
For code that wraps a Python class for C++, see `openage/util/fslike_cpp.pyx` and `libopenage/util/fslikeobject.h`.


Notes on the GIL
----------------

The GIL must be acquired for any Python functionality (even as simple as `PyErr_Occurred`).

However, GIL-safety is guaranteed by the combination of Cython, the pyinterface code, and the fact that libopenage doesn't link against Python itself / include any Python headers.

Any code in `libopenage` can safely be run without the GIL; functions should be declared `nogil` or Cython won't allow you to call them inside `with nogil:` blocks.

Only functions that are marked `with gil` can be bound to `PyIfFunc` or `Func` objects; this ensures that the GIL is always re-acquired when jumping into Cython code.

__Never__ use raw function pointers in the interface; always use the `PyIfFunc` or `Func` objects; otherwise, you'll lose all safety guarantees.
