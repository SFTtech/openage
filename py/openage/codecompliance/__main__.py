# Copyright 2014-2014 the openage authors. See copying.md for legal info.

import os
import shutil

from . import util

from . import headerguards
from . import legal
from . import authors


def main():
    import argparse
    cli = argparse.ArgumentParser()
    cli.add_argument("--all", action="store_true",
                     help="do all checks")
    cli.add_argument("--headerguards", action="store_true",
                     help="check all header guards")
    cli.add_argument("--legal", action="store_true",
                     help="check whether all sourcefiles have legal headers")
    cli.add_argument("--test-git-years", action="store_true",
                     help=("when doing legal checks, test whether the "
                           "Copyright years actually match the git history. "
                           "this option takes a long time and is not enabled "
                           "by --all."))
    cli.add_argument("--authors", action="store_true",
                     help=("check whether all git authors are in copying.md. "
                           "repo must be a git repository."))

    args = cli.parse_args()

    # sanitize arguments

    if args.all:
        if args.headerguards or args.legal or args.authors:
            cli.error("no individual checks can be specified with --all")

        args.headerguards = True
        args.legal = True
        args.authors = True

    if not args.headerguards and not args.legal and not args.authors:
        cli.error("no checks were specified")

    has_git = bool(shutil.which('git'))
    is_git_repo = os.path.isdir('.git')

    if args.authors:
        if not has_git or not is_git_repo:
            # non-fatal fail
            print("can not check author list for compliance: git is required")
            args.authors = False

    if args.test_git_years:
        if not args.legal:
            cli.error("--test-git-years may only be passed with --legal")

        if not has_git or not is_git_repo:
            cli.error("--test-git-years requires git")

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
