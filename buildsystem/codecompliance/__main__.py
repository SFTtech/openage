# Copyright 2014-2018 the openage authors. See copying.md for legal info.

"""
Entry point for the code compliance checker.
"""

import argparse
import importlib
import os
import shutil
import subprocess
import sys


def parse_args():
    """ Returns the raw argument namespace. """

    cli = argparse.ArgumentParser()
    cli.add_argument("--fast", action="store_true",
                     help="do all checks that can be performed quickly")
    cli.add_argument("--all", action="store_true",
                     help="do all checks, even the really slow ones")
    cli.add_argument("--only-changed-files", metavar='GITREF',
                     help=("slow checks are only done on files that have "
                           "changed since GITREF."))
    cli.add_argument("--textfiles", action="store_true",
                     help="check text files for whitespace issues")
    cli.add_argument("--headerguards", action="store_true",
                     help="check all header guards")
    cli.add_argument("--cppstyle", action="store_true",
                     help=("check the cpp code style"))
    cli.add_argument("--pystyle", action="store_true",
                     help=("check whether the python code complies with "
                           "(a selected subset of) pep8."))
    cli.add_argument("--pylint", action="store_true",
                     help=("run pylint on the python code"))
    cli.add_argument("--filemodes", action="store_true",
                     help=("check whether files in the repo have the "
                           "correct access bits (-> 0644) "))
    cli.add_argument("--authors", action="store_true",
                     help=("check whether all git authors are in copying.md. "
                           "repo must be a git repository."))
    cli.add_argument("--legal", action="store_true",
                     help="check whether all sourcefiles have legal headers")
    cli.add_argument("--test-git-change-years", action="store_true",
                     help=("when doing legal checks, test whether the "
                           "copyright year matches the git history."))

    cli.add_argument("--fix", action="store_true",
                     help=("try to automatically fix the found issues"))

    args = cli.parse_args()
    process_args(args, cli.error)

    return args


def process_args(args, error):
    """
    Sanitizes the given argument namespace, modifying it in the process.

    Calls error (with a string argument) in case of errors.
    """
    # this method is very flat; artificially nesting it would be bullshit.
    # pylint: disable=too-many-branches

    if args.fast or args.all:
        # enable "fast" tests
        args.headerguards = True
        args.legal = True
        args.authors = True
        args.textfiles = True
        args.cppstyle = True
        args.filemodes = True

    if args.all:
        # enable tests that take a bit longer

        args.pystyle = True
        args.pylint = True
        args.test_git_change_years = True

    if not any((args.headerguards, args.legal, args.authors, args.pystyle,
                args.cppstyle, args.test_git_change_years, args.pylint,
                args.filemodes, args.textfiles)):
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
        if not importlib.util.find_spec('pep8') and \
           not importlib.util.find_spec('pycodestyle'):

            error("pep8 or pycodestyle python module "
                  "required for style checking")

    if args.pylint:
        if not importlib.util.find_spec('pylint'):
            error("pylint python module required for linting")


def get_changed_files(gitref):
    """
    return a list of changed files
    """
    invocation = ['git', 'diff', '--name-only', '--diff-filter=ACMRTUXB',
                  gitref]

    try:
        file_list = subprocess.check_output(invocation)

    except subprocess.CalledProcessError as exc:
        raise Exception(
            "could not determine list of recently-changed files with git"
        ) from exc

    return set(file_list.decode('ascii').strip().split('\n'))


def main(args):
    """
    Takes an argument namespace as returned by parse_args.

    Calls find_all_issues(main args, list of files to consider)

    Returns True if no issues were found.
    """
    if args.only_changed_files:
        check_files = get_changed_files(args.only_changed_files)
    else:
        check_files = None

    auto_fixes = list()
    fixes_possible = False

    issues_count = 0
    for title, text, apply_fix in find_all_issues(args, check_files):
        issues_count += 1
        print("\x1b[33;1mWARNING\x1b[m {}: {}".format(title, text))

        if apply_fix:
            fixes_possible = True

            if args.fix:
                print("        This will be fixed automatically.")
                auto_fixes.append(apply_fix)
            else:
                print("        This can be fixed automatically.")

        # nicely seperate warnings
        print()

    if args.fix and auto_fixes:
        print("\x1b[33;1mApplying %d "
              "automatic fixes...\x1b[m" % len(auto_fixes))

        for auto_fix in auto_fixes:
            print(auto_fix())
            issues_count -= 1

        print()

    if issues_count > 0:
        plural = "s" if issues_count > 1 else ""

        if args.fix and auto_fixes:
            remainfound = "remain%s" % plural
        else:
            remainfound = ("were" if issues_count > 1 else "was") + "found"

        print("==> \x1b[33;1m{num} issue{plural}\x1b[m {remainfound}."
              "".format(num=issues_count,
                        plural=plural,
                        remainfound=remainfound))

        if not args.fix and fixes_possible:
            print("When invoked with --fix, I can try"
                  "to automatically resolve some of the issues.\n")

    return issues_count == 0


def find_all_issues(args, check_files=None):
    """
    Invokes all the individual issue checkers, and yields their returned
    issues.

    If check_files is not None, all other files are ignored during the
    more resource-intense checks.
    That is, check_files is the set of files to verify.

    Yields tuples of (title, text) that are displayed as warnings.
    """
    if args.headerguards:
        from .headerguards import find_issues
        yield from find_issues('libopenage')

    if args.authors:
        from .authors import find_issues
        yield from find_issues()

    if args.pystyle:
        from .pystyle import find_issues
        yield from find_issues(check_files, ('openage', 'buildsystem'))

    if args.cppstyle:
        from .cppstyle import find_issues
        yield from find_issues(check_files, ('libopenage',))

    if args.pylint:
        from .pylint import find_issues
        yield from find_issues(check_files, ('openage', 'buildsystem'))

    if args.textfiles:
        from .textfiles import find_issues
        yield from find_issues(
            ('openage', 'libopenage', 'buildsystem', 'doc', 'legal'),
            ('.pxd', '.pyx', '.pxi', '.py',
             '.h', '.cpp', '.template',
             '', '.txt', '.md', '.conf',
             '.cmake', '.in', '.yml', '.supp', '.desktop'))

    if args.legal:
        from .legal import find_issues
        yield from find_issues(check_files,
                               ('openage', 'buildsystem', 'libopenage'),
                               args.test_git_change_years)

    if args.filemodes:
        from .modes import find_issues
        yield from find_issues(check_files, ('openage', 'buildsystem',
                                             'libopenage'))


if __name__ == '__main__':
    if main(parse_args()):
        sys.exit(0)
    else:
        sys.exit(1)
