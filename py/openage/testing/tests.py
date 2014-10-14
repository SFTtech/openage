# tests for the openage.testing package
import sys
from . import testing


def test_testspec():
    """
    tests whether the testspec exists, and is well-formed
    """
    tests, demos = testing.read_testspec()
    testcount, democount = len(tests), len(demos)


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


def test_run_demo():
    """
    tests testing.run_demo
    """
    demoname = "openage.testing.tests.sample_demo"

    try:
        testing.run_demo([demoname, "--demoarg=5"])
    except Exception as e:
        raise Exception("demo execution failed") from e

    if sample_demo.suppliedarg != 5:
        raise Exception("expected demo to set sample_demo.suppliedarg=5")


def sample_demo(argv):
    """
    sample demo
    """
    import argparse
    ap = argparse.ArgumentParser(prog=argv[0])
    ap.add_argument("--demoarg", type=int, required=True)
    args = ap.parse_args(argv[1:])

    sample_demo.suppliedarg = args.demoarg


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
