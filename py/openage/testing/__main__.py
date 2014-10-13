import argparse
import sys


def main():
    ap = argparse.ArgumentParser(description=(
        "runs test methods in the python code. "
        "designed to work with the cmake script ('make test', "
        "see [buildsystem/testing.cmake])"))

    ap.add_argument("--list-tests", "-l", action="store_true",
                    help=("print all known tests (tests that have been "
                          "registered via add_test_py in a CMakeLists.txt "
                          "file)"))
    ap.add_argument("--all-tests", "-a", action="store_true",
                    help=("run all tests; fail if at least one of them has "
                          "failed"))
    ap.add_argument("--test", "-t", nargs='*',
                    help=("run the given test; fail if the test fails"))

    args = ap.parse_args()

    if args.all_tests and args.test:
        ap.error("--all-tests and --test are conflicting")

    if args.all_tests or args.list_tests:
            tests = read_testspec()

    if args.list_tests:
        maxtestnamelen = max(len(testname) for testname in tests)
        for testname, testdesc in tests.items():
            print("%s  %s" % (testname.ljust(maxtestnamelen), testdesc))

    if args.all_tests:
        for test in tests:
            run_test(test)

    for test in args.test:
        run_test(test)


if __name__ == '__main__':
    main()
