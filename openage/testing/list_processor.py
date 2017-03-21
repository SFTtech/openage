# Copyright 2015-2017 the openage authors. See copying.md for legal info.

""" Processes the raw test lists from the testlist module. """


from collections import OrderedDict
from importlib import import_module

from ..util.strings import lstrip_once


def tests_and_demos(test_lister, demo_lister):
    """
    Yields tuples of (testname, type, description, condition_function)
    for given test and demo listers.

    A processing step between the raw lists in testlist, and get_all_tests().
    """

    def default_cond(_):
        """ default condition test to enable a test """
        return True

    for test in test_lister():
        if not isinstance(test, tuple):
            test = (test,)

        if not test:
            raise ValueError("empty test definition encountered")

        if len(test) == 3:
            condfun = test[2]
        else:
            condfun = default_cond

        if len(test) >= 2:
            desc = test[1]
        else:
            desc = ""

        name = test[0]

        yield name, 'test', desc, condfun

    for demo in demo_lister():
        name, desc = demo
        yield name, 'demo', desc, default_cond


def tests_and_demos_py():
    """ Invokes tests_and_demos() with the py-specific listers. """
    from .testlist import tests_py, demos_py
    for val in tests_and_demos(tests_py, demos_py):
        yield val


def tests_and_demos_cpp():
    """ Invokes tests_and_demos() with the C++-specific listers. """
    from .testlist import tests_cpp, demos_cpp
    for val in tests_and_demos(tests_cpp, demos_cpp):
        yield val


def get_all_tests_and_demos():
    """
    Reads the Python and C++ testspec.

    returns an OrderedDict of
    {(testname, type): conditionfun, lang, description, testfun}.

    type is in {'demo', 'test'},
    lang is in {'cpp', 'py'},
    conditionfun is a callable which determines if the test is
        to be run in the given environment
    description is a str, and
    testfun is callable and takes 0 args for tests / list(str) for demos.
    """
    from .cpp_testing import run_cpp_method

    result = OrderedDict()

    for name, type_, description, conditionfun in tests_and_demos_py():
        modulename, objectname = name.rsplit('.', maxsplit=1)

        try:
            module = import_module(modulename)
            func = getattr(module, objectname)
        except Exception as exc:
            raise ValueError("no such function: " + name) from exc

        try:
            name = lstrip_once(name, 'openage.')
        except ValueError as exc:
            raise ValueError("Unexpected Python test/demo name") from exc

        result[name, type_] = conditionfun, 'py', description, func

    for name, type_, description, conditionfun in tests_and_demos_cpp():
        if type_ == 'demo':
            def runner(args, name=name):
                """ runs the demo func, and ensures that args is empty. """
                if args:
                    raise ValueError("C++ demos can't take arguments. "
                                     "You should write a Python demo that "
                                     "calls to C++ then, with arguments.")
                run_cpp_method(name)
        else:
            def runner(name=name):
                """ simply runs the demo func. """
                run_cpp_method(name)

        try:
            name = lstrip_once(name, 'openage::')
        except ValueError as exc:
            raise ValueError("Unexpected C++ test/demo name") from exc

        result[name, type_] = conditionfun, 'cpp', description, runner

    return result
