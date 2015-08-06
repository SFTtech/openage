# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Testing code for exctranslate.pyx.
Also see the sister file, cpp/pyinterface/exctranslate_tests.h.
"""

from libopenage.pyinterface.exctranslate_tests cimport (
    err_py_to_cpp_helper,
    err_cpp_to_py_helper,
    bounce_call_py,
    bounce_call as bounce_call_cpp,
)

from libopenage.pyinterface.functional cimport Func0

import argparse
import traceback

from ..testing.testing import TestError, assert_value

from .exctranslate import CPPException


cdef void call_cpp_thrower() except * with gil:
    err_cpp_to_py_helper()


def cpp_to_py_demo(args):
    """
    Calls a C++ method that throws a complicated exception, and prints
    that exception.
    """
    cdef Func0[void] cpp_thrower

    cli = argparse.ArgumentParser("cpp_to_py_demo")
    cli.add_argument("bounce_count", type=int, default=0, nargs='?')
    cdef int bounce_count = cli.parse_args(args).bounce_count

    try:
        if bounce_count == 0:
            call_cpp_thrower()
        else:
            # the first bounce already happens in the bound function.
            cpp_thrower.bind0(call_cpp_thrower)
            bounce_call(cpp_thrower, bounce_count - 1)

    except CPPException as exc:
        print("translated exception:\n")
        traceback.print_exc()


def cpp_to_py(int bounce_count = 0):
    """
    Calls a C++ method that throws a exception, and tests whether
    that exception is properly translated.
    """
    cdef Func0[void] cpp_thrower

    try:
        if bounce_count == 0:
            call_cpp_thrower()
        else:
            # the first bounce already happens in the bound function.
            cpp_thrower.bind0(call_cpp_thrower)
            bounce_call(cpp_thrower, bounce_count - 1)

    except CPPException as exc:
        # everything looks good. store the exc object for later analysis.
        excobj = exc
    except Exception as exc:
        raise TestError(
            "Expected a CPPException, but got something else.") from exc
    else:
        raise TestError("Expected a CPPException, but method returned.")

    # now let's see about the detailed contents of excobj.
    assert_value(excobj.args[0], "foo")
    assert_value(excobj.typename, b"openage::error::Error")

    cause = getattr(excobj, "__cause__", None)

    assert_value(type(cause), TestError)

    causeofcause = getattr(cause, "__cause__", None)

    assert_value(type(causeofcause), CPPException)
    assert_value(causeofcause.args[0], "rofl")
    assert_value(causeofcause.typename, b"openage::error::Error")

    assert_value(getattr(causeofcause, "__cause__", None), None)


def cpp_to_py_bounce():
    """
    Like cpp_to_py, but constructs a huge stack before throwing, so the
    exception gets translated back and forth quite often.
    """
    cpp_to_py(42)


class Bar(Exception):
    """ For testing (via throw_bar()) """
    pass


def throw_bar(cause, ctr=3):
    """ Invoked by the method below, to add some flavor to the stack trace. """
    if ctr > 0:
        throw_bar(cause, ctr - 1)
    else:
        raise Bar("bar") from cause


cdef void py_exc_raiser() except * with gil:
    """ Raises some Py exceptions for C++ to analyze and convert to C++. """
    try:
        raise TestError("foo")
    except Exception as exc:
        throw_bar(exc)


cdef void bounce_call(Func0[void] func, int times) except * with gil:
    """
    Creates a callstack consisting of 'times' alterations of this function
    and its C++ counterpart.
    On the top of that stack, calls func.
    """
    if times <= 0:
        func.call()
    else:
        bounce_call_cpp(func, times - 1)


def setup():
    """ Installs the functions defined here in their PyFunc pointers. """
    err_py_to_cpp_helper.bind0(py_exc_raiser)
    bounce_call_py.bind0(bounce_call)
