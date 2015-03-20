Overview
--------

There are various supported kinds of tests:

 - cpp tests
 - py tests
 - py doctests

All tests are run automatically by Travis, via `python3 -m openage.testing -a`.

You are encouraged to write tests for all your contributions, as well as other components that currently lack testing.

In addition to testing, openage supports _demos_:

 - cpp demos
 - py demos

As opposed to tests, demos are run manually and individually by the user.
They usually produce lots of output on stdout or may even be interactive. Python demos even accept an `argv` parameter.

All tests must be registered in `openage/testing/testlist.py` (else the game won't know about them).

Also see `python3 -m openage.testing --help`.

C++ tests
---------

C++ tests are simple `void()` functions somewhere in the `openage` namespace.

They shall return on success, and raise `openage::testing::TestError` on failure.

They shall not be declared in a header file; instead, add them to `openage/testing/testlist.py`.

The header `cpp/testing/testing.h` provides `TestError` and some convenience macros:

 - `TESTFAIL`
    throws `TestError`
 - `TESTFAILMSG("a" << "b")`
    throws `TestError("AB")`
 - `TESTEQUALS(left, right)`
    evaluates `left` and `right`
    throws `TestError(left)` if `left != right`
    throws `TestError(exc)` if a non-`TestError` exception `exc` is raised.
 - `TESTTHROWS(expr)`
    evaluates `expr`, catching any exception, including `TestError`.
    raises `TestError` if no exception was catched.

Example test function:

    void test_prime() {
        is_prime(23) or TESTFAIL;
        is_prime(42) and TESTFAIL;
    }

Python tests
------------

Python tests are simple argument-less functions somewhere in the `openage` package.

They shall return `None` on success, and raise `openage.testing.TestError` on failure.

Add their names to `openage/testing/testlist.py`.

The module `openage.testing.testing` provides `TestError` and some convenience functions:

 - `assert_result(callable, expected)`
    evaluates `callable()`, and checks whether its result equals `expected`.
    raises `TestError` if the result is wrong, or if an exception has occurred.
    more complex checking of the result is possible via `validator`; consult the docs.
 - `assert_raises(callable, expected)`
    evaluates `callable()`, and checks whether it raises an exception of type `expected`.
    raises `TestError` otherwise.

You may define tests in `.pyx` files.

Example test function:

    def test_prime():
        assert_result(is_prime(23), True)
        assert_result(is_prime(42), True)

Python doctests
---------------

[Doctests](https://docs.python.org/3.4/library/doctest.html) are an integrated feature of Python.

They defined in function and module docstrings, are extremely lightweight and also serve as documentation.

Simply add the name of a Python module to `openage/testing/testlist.py`, and all doctests in that module will run.

Example doctest for a function:

    def is_prime(p):
        """
        High-performance, state-of-the-art primality tester.

        > is_prime(23)
        True
        > is_prime(42)
        False
        """
        return not any(p % x == 0 for x in range(2, p))

C++ demos
---------

Technically, those are very much like `C++` tests. In fact, the only difference to tests is the section in `openage/testing/testlist.py` where they are declared.

C++ demos don't support `argv`; if you want that, make it a Python demo in a `.pyx` file and do the argparsing in Python; the Python demo function can then easily call any C++ function using the Python interface.

Python demos
------------

Similar to Python tests, but have one argument, `argv`. Pass arguments in the invocation:

    python3 -m openage.testing -d prime_demo 100

Example demo:

    def prime_demo(argv):
        import argparse
        cli = argparse.ArgumentParser()
        cli.add_argument(max_number, type=int)
        args = cli.parse_args(argv)

        for p in range(2, max_number):
            if is_prime(p):
                print(p)
