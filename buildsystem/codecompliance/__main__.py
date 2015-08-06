# Copyright 2014-2015 the openage authors. See copying.md for legal info.

"""
Entry point for the code compliance checker.
"""

import argparse
import importlib
import os
import shutil
from subprocess import Popen, PIPE


def parse_args():
    """ Returns the raw argument namespace. """

    cli = argparse.ArgumentParser()
    cli.add_argument("--fast", action="store_true",
                     help="do all checks that can be preformed quickly")
    cli.add_argument("--all", action="store_true",
                     help="do all checks, even the really slow ones")
    cli.add_argument("--only-changed-files", metavar='GITREF',
                     help=("slow checks are only done on files that have "
                           "changed since GITREF."))
    cli.add_argument("--textfiles", action="store_true",
                     help="check text files for whitespace issues")
    cli.add_argument("--headerguards", action="store_true",
                     help="check all header guards")
    cli.add_argument("--pystyle", action="store_true",
                     help=("check whether the python code complies with "
                           "(a selected subset of) pep8."))
    cli.add_argument("--pylint", action="store_true",
                     help=("run pylint on the python code"))
    cli.add_argument("--authors", action="store_true",
                     help=("check whether all git authors are in copying.md. "
                           "repo must be a git repository."))
    cli.add_argument("--legal", action="store_true",
                     help="check whether all sourcefiles have legal headers")
    cli.add_argument("--test-git-change-years", action="store_true",
                     help=("when doing legal checks, test whether the "
                           "copyright year matches the git history."))

    args = cli.parse_args()
    process_args(args, cli.error)

    return args


def process_args(args, error):
    """
    Sanitizes the given argument namespace, modifying it in the process.

    Calls error (with a string argument) in case of errors.
    """
    if args.fast or args.all:
        # enable "fast" tests
        args.headerguards = True
        args.legal = True
        args.authors = True
        args.textfiles = True

    if args.all:
        # enable tests that take a bit longer

        args.pystyle = True
        args.pylint = True
        args.test_git_change_years = True

    if not any((args.headerguards, args.legal, args.authors, args.pystyle,
                args.test_git_change_years, args.pylint)):
        error("no checks were specified")

    has_git = bool(shutil.which('git'))
    is_git_repo = os.path.isdir('.git')

    if args.only_changed_files and not all((has_git, is_git_repo)):
        error("can not check only changed files: git is required")

    if args.authors:
        if not all((has_git, is_git_repo)):
            # non-fatal fail
            print("can not check author list for compliance: git is required")
            args.authors = False

    if args.test_git_change_years:
        if not args.legal:
            error("--test-git-change-years may only be passed with --legal")

        if not all((has_git, is_git_repo)):
            error("--test-git-change-years requires git")

    if args.pystyle:
        try:
            importlib.import_module('pep8')
        except ImportError:
            error("pep8 python module required for style checking")


def main(args):
    """
    Takes an argument namespace as returned by parse_args.
    Returns True if no issues were found.
    """
    if args.only_changed_files:
        gitref = args.only_changed_files
        invocation = ['git', 'diff', '--name-only', '--diff-filter=ACMRTUXB',
                      gitref]

        proc = Popen(invocation, stdout=PIPE)
        check_files = proc.communicate()[0]

        if proc.returncode != 0:
            print("could not determine list of recently-changed files")
            return False

        check_files = check_files.decode('ascii').strip().split('\n')
    else:
        check_files = None

    issues_found = False
    for title, text in find_all_issues(args, check_files):
        issues_found = True
        print("\x1b[33;1mWARNING\x1b[m {}: {}".format(title, text))

    return not issues_found


def find_all_issues(args, check_files=None):
    """
    Invokes all the individual issue checkers, and yields their returned
    issues.

    If check_files is not None, all other files are ignored during the
    more resource-intense checks.
    """
    if args.headerguards:
        from .headerguards import find_issues
        yield from find_issues('libopenage', 'OPENAGE_')

    if args.authors:
        from .authors import find_issues
        yield from find_issues()

    if args.pystyle:
        from .pystyle import find_issues
        yield from find_issues(check_files, ('openage', 'buildsystem'))

    if args.pylint:
        from .pylint import find_issues
        yield from find_issues(check_files, ('openage', 'buildsystem'))

    if args.textfiles:
        from .textfiles import find_issues
        yield from find_issues(
            ('openage', 'libopenage', 'buildsystem', 'doc', 'legal'),
            ('.pxd', '.pyx', '.pxi', '.py',
             '.h', '.cpp', '.template',
             '', '.txt', '.conf',
             '.cmake', '.in', '.yml', '.supp', '.desktop'))

    if args.legal:
        from .legal import find_issues
        yield from find_issues(check_files,
                               ('openage', 'buildsystem', 'libopenage'),
                               args.test_git_change_years)


if __name__ == '__main__':
    if main(parse_args()):
        exit(0)
    else:
        exit(1)
