# Copyright 2015-2017 the openage authors. See copying.md for legal info.

from libcpp.string cimport string

from libopenage.util.enum cimport Enum
from libopenage.util.enum_test cimport (
    testtype,

    foo,
    bar,
)


from .testing import TestError, assert_value


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

    assert_value(test.name(), b"openage::util::tests::testtypes::foo")
