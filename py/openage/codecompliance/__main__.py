# Copyright 2014-2014 the openage authors. See copying.md for legal info.

import os
import shutil

from . import authors
from . import headerguards
from . import legal
from . import pystyle
from . import util


def main():
    import argparse
    cli = argparse.ArgumentParser()
    cli.add_argument("--quick", action="store_true",
                     help="do all checks that can be preformed quickly")
    cli.add_argument("--all", action="store_true",
                     help="do all checks")
    cli.add_argument("--headerguards", action="store_true",
                     help="check all header guards")
    cli.add_argument("--legal", action="store_true",
                     help="check whether all sourcefiles have legal headers")
    cli.add_argument("--authors", action="store_true",
                     help=("check whether all git authors are in copying.md. "
                           "repo must be a git repository."))
    cli.add_argument("--pystyle", action="store_true",
                     help=("check whether all git authors are in copying.md. "
                           "repo must be a git repository."))
    cli.add_argument("--test-git-years", action="store_true",
                     help=("when doing legal checks, test whether the "
                           "Copyright years actually match the git history. "
                           "this option takes a long time and is not enabled "
                           "by --all."))

    args = cli.parse_args()

    # sanitize arguments

    if args.quick:
        args.headerguards = True
        args.legal = True
        args.authors = True

    if args.all:
        args.headerguards = True
        args.legal = True
        args.authors = True
        args.pystyle = True
        args.test_git_years = True

    if not any((args.headerguards, args.legal, args.authors, args.pystyle,
                args.test_git_years)):
        cli.error("no checks were specified")

    has_git = bool(shutil.which('git'))
    is_git_repo = os.path.isdir('.git')

    if args.authors:
        if not all((has_git, is_git_repo)):
            # non-fatal fail
            print("can not check author list for compliance: git is required")
            args.authors = False

    if args.test_git_years:
        if not args.legal:
            cli.error("--test-git-years may only be passed with --legal")

        if not all((has_git, is_git_repo)):
            cli.error("--test-git-years requires git")

    if args.pystyle:
        try:
            import pep8
        except ImportError:
            cli.error("pep8 python module required for style checking")

    def find_issues():
        if args.headerguards:
            for x in headerguards.find_issues('cpp', 'OPENAGE_'):
                yield x
            for x in headerguards.find_issues('py', 'PYEXT_'):
                yield x

        if args.legal:
            dirstocheck = ('cpp', 'py', 'buildsystem')
            for x in legal.find_issues(dirstocheck, args.test_git_years):
                yield x

        if args.authors:
            for x in authors.find_issues():
                yield x

        if args.pystyle:
            for x in pystyle.find_issues(('py',)):
                yield x

    ok = True
    for issue in find_issues():
        ok = False
        issue.print()

    return ok


if __name__ == '__main__':
    if main():
        exit(0)
    else:
        exit(1)
