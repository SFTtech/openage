# Copyright 2015-2018 the openage authors. See copying.md for legal info.

from cython.operator cimport dereference as deref

from libcpp.string cimport string

from libopenage.util.enum_test cimport (
    testenum,

    foo,
    bar,
)


from .testing import TestError, assert_value


def enum():
    cdef testenum test = foo

    if test != foo:
        raise TestError()

    if test == bar:
        raise TestError()

    if foo == bar:
        raise TestError()

    assert_value(test.get().name, b"foo")
    assert_value(test.get().numeric, 1)
    assert_value(test.get().stuff, b"foooooooooooooooooo")

    assert_value(foo > bar, False)
    assert_value(foo < bar, True)
    assert_value(foo >= bar, False)
    assert_value(foo <= bar, True)
