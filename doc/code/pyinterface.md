openage C++ <-> Python interface
================================

openage consists of python modules, which contain the program entry point,
and the library `libopenage.so`, which contains all C++ code.

Cython is used for glue code.


Cython crash course
-------------------


### Python with types

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
    cdef Rectangle r
    r.h = 5
    r.w = 6
    return square(r.size())
```

More language information [is in the official documentation](https://cython.readthedocs.io/en/latest/src/userguide/language_basics.html).

`.pyx` files are translated to `.cpp` by Cython ("cythonized") as part of the
openage build process; syntax errors are shown in this step.

Each `.cpp` file is then compiled to a Python extension module, which may be
used from everywhere.

- `def` functions and `cdef class` classes can be used from Python
- `cdef` functions can't be used directly
  - suitable for storage in a `C` function pointer
  - callable from `def` functions in the `.pyx` file

```
$ cython --cplus -3 test.pyx
$ g++ -shared -fPIC $(python3-config --includes) test.cpp -o test.so
$ python3
>>> import test
>>> test.foo()
900
```

### Interface definitions

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
    # invokes the c implementation according to the
    # interface defined in a .pxd file
    return c_atoi(s)
```

### pxd generation

`openage` has a helper, `pxdgen`, which auto-generates `.pxd` files for `.h`
files in the `libopenage/` subdirectory, from `pxd:` annotations in these files, as part
of the build system.

The pxd annotations are really simple; just have a look at some of the headers.
You'll find the generated `.pxd` files next to the C++ header files.

To `cimport` a class Foo that was pxd-annotated in `util/foo.h`, type

``` cython
from libopenage.util.foo cimport Foo
```

Cython [ships lots of `.pxd` files](https://github.com/cython/cython/tree/master/Cython/Includes) for most C, C++ and CPython functions:

``` cython
from libc.math cimport sin
from libcpp.vector cimport vector

cdef vector[float] vector_sin(vector[float]& args):
    return [sin(arg) for arg in args]

print(vector_sin(range(10)))
```

From time to time, it may be useful to have a look at the generated `.cpp` file,
especially if you require more exotic functionality or something doesn't work
out as you expected. For that purpose, `.html` files are generated next to the `.cpp` files.


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

}
```

`libopenage/example.h`

``` cpp
// pxd: from libcpp.string cimport string
#include <string>
#include "util/compiler.h"

namespace openage {

/**
 * The famous foo function. Warning: might bar occasionally.
 *
 * pxd:
 *
 * int foo(int arg0, string arg1) except +
 */
OAAPI int foo(int arg0, std::string arg1);

}
```

_Always_ pxd-declare your functions as `except +` ("may potentially rise a C++ exception"), unless the C++ function is marked `noexcept` itself.
If a function that is not declared `except +` throws anyways, the entire CPython interpreter is likely to be shredded.
(Sidenote: the `except +` annotation enables openage-specific exception translation. To see how this is used, look in the `exctranslate_tests.pyx/cpp`)

To enable `pxdgen`, `cmake` must be informed about the `pxd`-annotated header file:

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
Thus, `libopenage` must find its way back to Python:

* By `pyinterface::PyIfFunc` function pointers that are filled by Cython during initialization.
* By `py::PyObj` objects that hold a Python object. It may be callable.


### Calling directly to C++

Any Python object can be stored in a `PyObj` (`from libopenage.pyinterface.pyobject`),
which C++ can use to do calls to Python callables.

This works in the way that you create C++ objects in Cython and then
[call a C++ function](#calling-c-functions-from-python) where you pass it.


``` c++
// pxd: from libcpp.string cimport string
#include <string>

// pxd: from libopenage.pyinterface.pyobject cimport PyObj
#include "libopenage/pyinterface/pyobject.h"


/**
 * pxd:
 *
 * cppclass demo_struct:
 *     PyObj obj
 *     string text
 */
struct OAAPI demo_struct {
    py::PyObj obj;
    std::string text;
};

/**
 * pxd: int cpp_function(lol_struct arg, int another_arg) except +
 */
void cpp_function(lol_struct &arg, int another_arg) {
    // native data can directly be used
    std::cout << "native_arg: " << arg.text << std::endl;

    // call the python
    // with automatic argument conversion!
    std::cout << "python call: "
        << arg.obj->getattr("py_func").call("some binary", another_arg).str()
        << std::endl;
}
```

``` cython
from libopenage.pyinterface.pyobject cimport PyObj
from libopenage.main cimport demo_struct, cpp_function

class TestClass:
    def __init__(self):
        self.some_member = "rofl"

    def py_func(self, arg0, arg1):
        return "test: 0={} 1={} 2={}".format(self.some_member, arg0, arg1)

def entry():
    # create the object and deliver it to c++
    test_obj = TestClass()

    # python object wrapping for c++
    cdef PyObj pyobj_wrapped = PyObj(<PyObject*> some_object)

    # create a c++ object
    cdef demo_struct cppobj
    cppobj.obj = pyobj_wrapped
    cppobj.text = "behold the automatic type conversion!"

    # call to c++
    cpp_function(cpp_obj, 1337)
```

This means that Python calls to C++ with `cpp_function`,
then C++ calls back to Python to the `py_function`.

C++ can access a `PyObj` in many more ways, perform casts, etc.


### Registering Python functions in C++

To access call Python functions some time later, they can be remembered in C++:

Any `cdef` functions can be stored in C++ function pointers.
The `openage::pyinterface::PyIfFunc` type has been created for this purpose;
it allows *binding arguments* and makes sure that the pointer is properly initialized.

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
extern OAAPI PyIfFunc<float, int, std::string> bar;
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

std::cout << openage::bar.call(5, "test") << std::endl;
```

The __only__ way of accessing Python code from C++ should be via the `Func` or `PyIfFunc` function wrappers, as those
guarantee that exceptions are properly translated, among other things.


Real-life examples
------------------

* For code that wraps a C++ class for Python, see:
  * `openage/cabextract/lzxd.pyx`
  * `libopenage/util/lzxd.h`
* For code that wraps a Python class for C++, see:
  * `libopenage/util/fslike/path.h`
  * `openage/util/fslike/cpp.pyx`
* Search for `.pyx` files in the repo


Notes on the GIL
----------------

The GIL must be acquired for any Python functionality (even as simple as `PyErr_Occurred`).

However, GIL-safety is guaranteed by the combination of Cython, the pyinterface code,
and the fact that `libopenage` doesn't link against Python itself / include any Python headers.

Any code in `libopenage` can safely be run without the GIL.

Only functions that are marked `with gil` can be bound to `PyIfFunc` or `Func` objects;
this ensures that the GIL is always re-acquired when jumping into Cython code.

__Never__ use raw function pointers in the interface;
always use the `PyIfFunc` or `Func` objects; otherwise, you'll lose all safety guarantees.

The `OAAPI` macro
-----------------

`OAAPI` marks the DLL entry-points which is necessary for the Windows build.
All pxd interface functions, classes, and `extern` objects must be declared with `OAAPI` in the header file.
See the samples above to understand the exact position of insertion for each of those.
This can be ignored for functions defined inline in the header.
