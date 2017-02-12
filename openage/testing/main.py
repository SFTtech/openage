# Copyright 2014-2017 the openage authors. See copying.md for legal info.

""" CLI module for running all tests. """

import argparse

from ..util.strings import format_progress

from .testing import TestError
from .list_processor import get_all_tests_and_demos


def print_test_list(test_list):
    """
    Prints a list of all tests and demos in test_list.
    """
    namelen = max(len(name) for name, _ in test_list.keys())

    for current_type in ['test', 'demo']:
        for (name, type_), (_, lang, desc, _) in test_list.items():
            if type_ == current_type:
                print("[%s %3s] %-*s  %s" % (type_, lang, namelen, name, desc))

    print("")
    print("To see how to run them, add --help to your invocation!")
    print("")
    print("Remember: Testing is the future, and the future starts with: You.")
    print("")


def init_subparser(cli):
    """ Initializes the subparser for the testing command. """
    cli.set_defaults(entrypoint=main)

    cli.add_argument("--list", "-l", action='store_true',
                     help="list all tests and demos")
    cli.add_argument("--run-all-tests", "-a", action='store_true',
                     help="run all tests")
    cli.add_argument("--have-assets", action='store_true',
                     help="additionally, run tests that need asset files")

    cli.add_argument("--demo", "-d", nargs=argparse.REMAINDER,
                     help=("run the given demo; the remaining arguments "
                           "are passed to the demo."))
    cli.add_argument("test", nargs='*', help="run this test")


def process_args(args, error):
    """ Processes the given args, detecting errors. """
    if not args.run_all_tests and not args.demo and not args.test:
        args.list = True

    if args.have_assets and not args.run_all_tests:
        error("you have to run all tests, "
              "otherwise I don't care if you have assets")

    if args.run_all_tests:
        if args.test or args.demo:
            error("can't run individual test or demo when running all tests")

    if args.test and args.demo:
        error("can't run a demo _and_ tests")

    # link python and c++ so it hopefully works when testing
    from openage.cppinterface.setup import setup
    setup()

    test_list = get_all_tests_and_demos()

    # the current test environment can have influence on the tests itself.
    test_environment = {
        "has_assets": args.have_assets,
    }

    # if we wanna run all the tests, only run the ones that
    # are happy with the environment
    if args.run_all_tests:
        for (name, test_type), (test_condition, _, _, _) in test_list.items():
            if test_type == "test" and test_condition(test_environment):
                args.test.append(name)

    # double-check for unknown tests and demos
    for test in args.test:
        if (test, 'test') not in test_list:
            error("no such test: " + test)

    if args.demo:
        if (args.demo[0], 'demo') not in test_list:
            error("no such demo: " + args.demo[0])

    return test_list


def main(args, error):
    """ CLI main method. """
    test_list = process_args(args, error)

    if args.list:
        print_test_list(test_list)

    if args.test:
        failed = 0

        for idx, name in enumerate(args.test):
            _, lang, _, testfun = test_list[name, 'test']

            print("\x1b[32m[%s]\x1b[m %3s %s" % (
                format_progress(idx, len(args.test)),
                lang,
                name))

            try:
                testfun()
            except BaseException as exc:
                print("\x1b[31;1mTest failed\x1b[m")
                if not isinstance(exc, TestError):
                    print("\x1b[31;1mUnexpected exception\x1b[m")
                failed += 1
                import traceback
                traceback.print_exc()

        if failed:
            print("\x1b[31;1m%d out of %d tests have failed\x1b[m" % (
                failed,
                len(args.test)))

            exit(1)

    if args.demo:
        _, _, demofun = test_list[args.demo[0], 'demo']
        exit(demofun(args.demo[1:]))
