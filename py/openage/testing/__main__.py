# Copyright 2014-2014 the openage authors. See copying.md for legal info.

import argparse
import sys

from . import testing


def main():
    ap = argparse.ArgumentParser(description=(
        "runs test methods in the python code. "
        "designed to work with the cmake script ('make test', "
        "see [buildsystem/testing.cmake])"))

    ap.add_argument("--list", "-l", action="store_true",
                    help=("print all known tests and demos"))
    ap.add_argument("--all-tests", "-a", action="store_true",
                    help=("run all tests; fail if at least one of them has "
                          "failed"))
    ap.add_argument("--demo", "-d", nargs=argparse.REMAINDER,
                    help=("run the given demo. all following arguments are"
                          "interpreted as arguments to the demo."))
    ap.add_argument("--test", "-t", nargs='*',
                    help=("run the given test; fail if the test fails"))

    args = ap.parse_args()

    if args.all_tests and args.test:
        ap.error("--all-tests and --test are conflicting")

    if args.all_tests or args.list:
            tests, demos = testing.read_testspec()

    if args.list:
        maxnamelen = 0
        for testname in tests:
            maxnamelen = max(maxnamelen, len(testname))
        for demoname in demos:
            maxnamelen = max(maxnamelen, len(demoname))

        for testname, testdesc in sorted(tests.items()):
            print("[test] %s  %s" % (testname.ljust(maxnamelen), testdesc))

        for demoname, demodesc in sorted(demos.items()):
            print("[demo] %s  %s" % (demoname.ljust(maxnamelen), demodesc))

    if args.all_tests:
        for test in tests:
            testing.run_test(test)

    if args.test:
        for test in args.test:
            testing.run_test(test)

    if args.demo:
        testing.run_demo(args.demo)


if __name__ == '__main__':
    main()
