# Copyright 2014-2018 the openage authors. See copying.md for legal info.

""" Testing utilities, such as TestError, assert_value, assert_raises. """

from contextlib import contextmanager


class TestError(Exception):
    """
    Raised by assert_value and assert_raises, but may be manually raised
    to indicate a test error.
    """
    pass


def assert_value(value, expected=None, validator=None):
    """
    Checks 'value' for equality with 'expected', or, if validator is given,
    checks bool(validator(value)) == True. Raises TestError on failure.

    Example usage:

    assert_value(fibonacci(0), 1)
    """
    if expected is not None and validator is not None:
        raise ValueError("can't have both 'expected' and 'validator'")

    if validator is None:
        success = value == expected
    else:
        success = validator(value)

    if success:
        return

    raise TestError("unexpected result: " + repr(value))


def result(value):
    """
    Shall be called when a result is unexpectedly returned in an assert_raises
    block.
    """
    raise TestError("expected exception, but got result: " + repr(value))


@contextmanager
def assert_raises(expectedexception):
    """
    Context guard that asserts that a certain exception is raised inside.

    On successful execution (if the error failed to show up),
    result() shall be called.

    Example usage:

    # we expect fibonacci to raise ValueError for negative values.
    with assert_raises(ValueError):
        result(fibonacci(-3))
    """
    # pylint: disable=try-except-raise
    try:
        yield
    except expectedexception:
        return
    except TestError:
        raise
    except BaseException as exc:
        raise TestError("unexpected exception") from exc
    else:
        raise TestError("got neither an exception, nor a result")
