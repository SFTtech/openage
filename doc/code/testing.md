# Test system

## openage computer-enriched testing automation

### Tests

There are various supported kinds of tests:

 - cpp tests
 - py tests
 - py doctests

Tests run without user interaction to check for errors automatically.

All tests are run automatically by [Kevin](https://github.com/SFTtech/kevin/) for pullrequests.


You can invoke them with `./run test -a` or `make test`

Have a look at `./run test --help` for further options.


You are encouraged to write tests for all your contributions, as well as other components that currently lack testing.


### Demos

In addition to testing, openage supports _demos_:

 - cpp demos
 - py demos

As opposed to tests, demos are run manually and individually by the user.
They usually produce lots of output on stdout or may even be interactive. Python demos even accept an `argv` parameter.

All tests must be registered in `openage/testing/testlist.py` (else the game won't know about them).

Also see `python3 -m openage.testing --help`.


## Adding new tests

### C++ tests

C++ tests are simple `void()` functions somewhere in the `openage` namespace.

They shall return on success, and raise `openage::testing::TestError` on failure.

They shall not be declared in a header file; instead, add them to `openage/testing/testlist.py`.

The header `libopenage/testing/testing.h` provides `TestError` and some convenience macros:

 - `TESTFAIL`
    throws `TestError`
 - `TESTFAILMSG("a" << "b")`
    throws `TestError("ab")`
 - `TESTEQUALS(left, right)`
    evaluates `left` and `right`
    throws `TestError(left)` if `left != right`
    throws `TestError(exc)` if a non-`TestError` exception `exc` is raised.
 - `TESTTHROWS(expr)`
    evaluates `expr`, catching any exception, including `TestError`.
    raises `TestError` if no exception was catched.

Example test function:

``` cpp
void test_prime() {
    is_prime(23) or TESTFAIL;
    is_prime(42) and TESTFAIL;
}
```


### Python tests

Python tests are simple argument-less functions somewhere in the `openage` package.

They shall return `None` on success, and raise `openage.testing.TestError` on failure.

Add their names to `openage/testing/testlist.py`.

The module `openage.testing.testing` provides `TestError` and some convenience functions:

 - `assert_value(<expr>, expected)`
    checks whether expr == expected, and raises `TestError` if not.
 - `assert_raises(expected_exception_type)`
    a context guard that verifies that the named exception occurs inside;
    consult the example in `openage/testing/testing.py`.

You may define tests in `.pyx` files.

Example test function:

``` python
def test_prime():
    assert_value(is_prime(23), True)
    assert_value(is_prime(42), False)

    with assert_raises(ValueError):
        result(is_prime(-1337))
```


### Python doctests

[Doctests](https://docs.python.org/3/library/doctest.html) are an integrated feature of Python.

They defined in function and module docstrings, are extremely lightweight and also serve as documentation.

Simply add the name of a Python module to `openage/testing/testlist.py`, and all doctests in that module will run.

Example doctest for a function:

``` python
def is_prime(p):
    """
    High-performance, state-of-the-art primality tester.

    >>> is_prime(23)
    True
    >>> is_prime(42)
    False
    """
    return not any(p % x == 0 for x in range(2, p))
```

### C++ demos

Technically, those are very much like `C++` tests. In fact, the only difference to tests is the section in `openage/testing/testlist.py` where they are declared.

C++ demos don't support `argv`; if you want that, make it a Python demo in a `.pyx` file and do the argparsing in Python; the Python demo function can then easily call any C++ function using the Python interface.


### Python demos

Similar to Python tests, but have one argument, `argv`. Pass arguments in the invocation:

    python3 -m openage.testing -d prime_demo 100

Example demo:

``` python
def prime_demo(argv):
    import argparse
    cli = argparse.ArgumentParser()
    cli.add_argument('max_number', type=int)
    args = cli.parse_args(argv)

    for p in range(2, args.max_number):
        if is_prime(p):
            print(p)
```


## Why?

### Demos

Demos should be used to implement and develop new features.
You can directly call your code without having to launch up the whole engine.

Use demos while developing new things or improvements.


### Tests

All tests are run for each pull requests, so we can detect your change broke something.
Please try to write tests for everything you do.

This is our only way of automated regression detection.
