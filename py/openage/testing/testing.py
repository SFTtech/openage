# Copyright 2014-2014 the openage authors. See copying.md for legal info.

from .. import assets

testspec_asset = "tests_py"


def read_testspec():
    tests = {}
    demos = {}

    testspec_filename = assets.get_file(testspec_asset)
    with open(testspec_filename) as testspec:
        for test in testspec.read().split('\n'):
            test = test.strip()
            if not test or test.startswith('#'):
                continue

            try:
                testname, testtype, description = test.split(' ', maxsplit=2)
            except ValueError as e:
                raise Exception("testspec malformed: %s"
                                % testspec_filename) from e

            if testtype == 'demo':
                demos[testname] = description
            elif testtype == 'test':
                tests[testname] = description
            else:
                raise Exception("unknown test type: %s" % testtype)

    return tests, demos


def get_function(functionname):
    module, name = functionname.rsplit('.', maxsplit=1)

    namespace = {}
    try:
        exec("from %s import %s" % (module, name), namespace)
    except Exception as e:
        raise Exception("no such object: %s" % functionname) from e

    function = namespace[name]

    if not function:
        raise Exception("function object invalid: %s" % functionname)
    if not callable(function):
        raise Exception("not a function: %s" % functionname)

    return function


def run_test(testname):
    # try running the function
    get_function(testname)()


def run_demo(demo):
    # try running the demo
    get_function(demo[0])(demo)
