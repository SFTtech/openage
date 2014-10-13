# tests for the openage.testing package
from . import testing


def test_testspec():
    """
    tests whether the testspec exists, and is well-formed
    """
    testing.read_testspec()


def test_run_test():
    """
    tests testing.run_test
    """
    try:
        testing.run_test("openage.testing.tests.fail")
    except CustomException as e:
        # tests.fail was designed to fail with precisely this exception.
        # everything is fine.
        pass
    except Exception as e:
        raise Exception("test failed with wrong Exception") from e
    else:
        raise Exception("expected test of tests.fail to raise an Exception")

    try:
        testing.run_test("openage.testing.tests.succeed")
    except:
        raise Exception("expected test of tests.succeed to succeed") from e


class CustomException(Exception):
    pass


def fail():
    """
    designed to be used with testruntest

    do not use this
    """
    raise CustomException("this test is designed to fail.")

def succeed():
    """
    designed to be used with testruntest

    do not use this
    """
    pass
