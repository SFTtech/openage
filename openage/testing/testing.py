# Copyright 2014-2015 the openage authors. See copying.md for legal info.

""" Testing utilities, such as TestError, assert_result, assert_raises. """


class TestError(Exception):
    """
    Raised by the testing system when a test fails.
    """
    pass


def assert_result(functor, expected=None, validator=None):
    """
    Helper function that checks whether functor() returns the expected value.

    If validator is not None, the result is checked using validator.
    Validator must not raise an exception, and return a boolean value to
    indicate result validity. 'expected' is ignored.

    Otherwise, the result is checked for equality with 'expected'.

    Raises an exception for invalid results, and returns valid results.
    """
    try:
        result = functor()
    except BaseException as exc:
        raise TestError(
            "expected return value " + repr(expected) + ", "
            "but instead got an exception") from exc

    if validator is None:
        def default_validator(result):
            """ simply tests for equality with 'expected' """
            return result == expected
        validator = default_validator
    else:
        if expected is not None:
            raise TestError("can't specify both validator and expected")

    if not validator(result):
        raise TestError(
            "expected return value " + repr(expected) + ", "
            "but instead got return value " + repr(result))

    return result


def assert_raises(functor, expectedexception):
    """
    similar to assert_result, but asserts that the functor actually
    raises an Exception of a certain type.
    """
    try:
        result = functor()
    except expectedexception:
        return
    except BaseException as exc:
        raise TestError(
            "expected exception of type " + repr(expectedexception) + ", "
            "but got a different exception") from exc

    raise TestError(
        "expected exception of type " + repr(expectedexception) + ", "
        "but got return value " + repr(result))
