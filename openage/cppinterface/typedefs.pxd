# Copyright 2015-2016 the openage authors. See copying.md for legal info.

"""
Some types that are not available in Cython's shipped include files.
"""

cdef extern from "<memory>" namespace "std":
    cppclass unique_ptr[T]:
        void reset(T *ptr) nogil  # noexcept
        T *get() nogil            # noexcept


# in some cases, pointers can't be properly used for template arguments.
ctypedef void *voidptr
