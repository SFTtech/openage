# Copyright 2015-2017 the openage authors. See copying.md for legal info.

""" Processes the raw test lists from the testlist module. """


from collections import OrderedDict
from importlib import import_module

from ..util.strings import lstrip_once


def list_targets(test_lister, demo_lister, benchmark_lister):
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

    for benchmark in benchmark_lister():
        name, desc = benchmark
        yield name, 'benchmark', desc, default_cond


def list_targets_py():
    """ Invokes list_targets() with the py-specific listers. """
    from .testlist import tests_py, demos_py, benchmark_py
    for val in list_targets(tests_py, demos_py, benchmark_py):
        yield val


def list_targets_cpp():
    """ Invokes list_targets() with the C++-specific listers. """
    from .testlist import tests_cpp, demos_cpp, benchmark_cpp
    for val in list_targets(tests_cpp, demos_cpp, benchmark_cpp):
        yield val


def get_all_targets():
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

    for name, type_, description, conditionfun in list_targets_py():
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

    for name, type_, description, conditionfun in list_targets_cpp():
        if type_ == 'demo':
            def runner(args, name=name):
                """ runs the demo func, and ensures that args is empty. """
                if args:
                    raise ValueError("C++ demos can't take arguments. "
                                     "You should write a Python demo that "
                                     "calls to C++ then, with arguments.")
                run_cpp_method(name)
        elif type_ in ['test', 'benchmark']:
            def runner(name=name):
                """ simply runs the func. """
                run_cpp_method(name)
        else:
            raise ValueError("Unknown type " + type_)

        try:
            name = lstrip_once(name, 'openage::')
        except ValueError as exc:
            raise ValueError("Unexpected C++ test/demo name") from exc

        result[name, type_] = conditionfun, 'cpp', description, runner

    return result
