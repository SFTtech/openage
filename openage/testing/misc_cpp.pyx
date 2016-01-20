# Copyright 2015-2016 the openage authors. See copying.md for legal info.

from libcpp.string cimport string

from libopenage.util.enum cimport Enum
from libopenage.util.enum_test cimport (
    testtype,

    foo,
    bar,
)


from .testing import TestError


def enum():
    cdef testtype test = foo

    if not test.get().stuff == b"some text":
        raise TestError()

    if test != foo:
        raise TestError()

    if test == bar:
        raise TestError()

    if foo == bar:
        raise TestError()

    if test.name() != b"openage::util::tests::testtypes::foo":
        raise TestError()
