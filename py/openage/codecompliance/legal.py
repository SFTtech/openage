# Copyright 2014-2014 the openage authors. See copying.md for legal info.

import os
import itertools
import re
from subprocess import Popen, PIPE

from . import util


extensions_requiring_legal_headers = {
    '.h', '.cpp', '.py', ".cmake",
    '.h.in', '.cpp.in', '.py.in',
    '.h.template', '.cpp.template', '.py.template',
}

year = "([12][0-9][0-9][0-9])"
copyright = "Copyright {}-{}".format(year, year)
oacopyright = "{} the openage authors\\.".format(copyright)


class LegalIssue(Exception):
    pass


class ThirdPartyFile:
    def __init__(self, filename):
        self.filename = filename


def find_issues(dirname, test_git_years):
    """
    checks all source files for the required legal headers.
    """
    for f in os.listdir(dirname):
        if f.startswith('.'):
            continue

        name = os.path.join(dirname, f)

        if os.path.isdir(name):
            for x in find_issues(name, test_git_years):
                yield x
            continue

        for ending in extensions_requiring_legal_headers:
            if name.endswith(ending):
                # yay, we've found a file that needs checking
                with open(name) as f:
                    itera, iterb = itertools.tee(iter(f), 2)

                    try:
                        if is_native(itera, name, test_git_years):
                            continue
                        if is_third_party(iterb, name, test_git_years):
                            yield ThirdPartyFile(name)
                            continue

                        raise LegalIssue("\n\tFile has no valid legal header")

                    except LegalIssue as e:
                        yield util.Issue(
                            "legal header issue in {}".format(name),
                            "{}\n\tSee copying.md for guidelines and a "
                            "template".format(e.args[0]))


def find_third_party_file_list_issues(known_files):
    expr = re.compile('^ - `([^`]+)`.*$')
    listed_files = set()
    for line in open('copying.md').read().split('\n'):
        match = expr.match(line)
        if not match:
            continue

        filename = match.group(1)
        listed_files.add(filename)

    for filename in sorted(listed_files - known_files):
        for ext in extensions_requiring_legal_headers:
            if filename.endswith(ext):
                break
        else:
            break

        yield util.Issue(
            "third-party file listing issue",
            ("{}\n\tlisted in copying.md, but has no "
             "third-party license header.").format(filename))

    for filename in sorted(known_files - listed_files):
        yield util.Issue("third-party file listing issue",
                         ("{}\n\thas a third-party license header, but isn't "
                          "listed in copying.md").format(filename))


def getnextline(iterator):
    line = next(iterator, "EOF").strip()

    commentsequences = ('//', '/**', '/*', '*/', '*', '#')
    for seq in commentsequences:
        if line.startswith(seq):
            line = line[len(seq):]
            break

    return line.strip()


def testline(expr, line):
    return re.match("^{}$".format(expr), line)


def test_git_first_last_year(filename, lineno,
                             hdr_first_year, hdr_last_year):

    invocation = ['git',
                  'log',
                  '--follow',
                  '--date=short',
                  '--format=%ad',
                  '--',
                  filename]

    proc = Popen(invocation, stdout=PIPE)
    output = proc.communicate()[0].decode('utf-8', errors='ignore').strip()

    if proc.returncode != 0 or not output:
        return

    output = output.split('\n')

    if hdr_first_year != output[-1][:4]:
        raise LegalIssue((
            "Line {}:\n"
            "\tBad first copyright year in legal header:\n"
            "\texpected {} (from git history start {})\n"
            "\tfound    {}").format(
                lineno,
                output[-1][:4], output[-1],
                hdr_first_year))

    if hdr_last_year != output[0][:4]:
        raise LegalIssue((
            "Line {}:\n"
            "\tBad last copyright year in legal header:\n"
            "\texpected {} (from git history end {})\n"
            "\tfound    {}").format(
                lineno,
                output[0][:4], output[0],
                hdr_last_year))


def is_native(it, filename, test_git_years):
    headerre = ("{} See copying\\.md for legal info.".format(oacopyright))

    while True:
        line = getnextline(it)
        if not line:
            # allow empty lines
            continue

        if line.startswith("!"):
            # allow shebang lines
            continue

        if line == "EOF":
            # the file contains no actual code; it doesn't require a
            # license.
            return True

        match = testline(headerre, line)
        if match:
            if test_git_years:
                # let's see whether the years are actually correct
                test_git_first_last_year(filename,
                                         1,
                                         match.group(1),
                                         match.group(2))
            return True
        else:
            lowerline = line.lower()
            if "copyright" in lowerline or "copying" in lowerline:
                # it seems that you tried (but failed)
                raise LegalIssue((
                    "Line 1:\n"
                    "\texpected 'Copyright 20XX-20YY the openage authors. "
                    "See copying.md for legal info.'\n"
                    "\tfound    {}").format(repr(line)))
            else:
                # no header found
                return False

        return bool(testline(headerre, line))


def is_third_party(it, filename, test_git_years):
    line = getnextline(it)
    if not testline("This file was (taken|adapted) from .*,", line):
        if (("this file was taken from" in line.lower() or
                "this file was adapted from" in line.lower())):

            # it seems like you tried, but failed.
            raise LegalIssue((
                "Line 1:\n"
                "\tfor third-party legal header: "
                "expected origin description\n"
                "\tfound    {}").format(repr(line)))
        return False

    # the file _is_ a third party header.
    # now we test whether all other required lines are correct.
    # if not, we'll raise a LegalIssue.

    line = getnextline(it)
    if not testline("{} .*".format(copyright), line):
        raise LegalIssue((
            "Line 2:\n"
            "\texpected copyright line\n"
            "\tfound    {}").format(repr(line)))

    while True:
        # allow any number of further copyright lines
        line = getnextline(it)
        if not testline("{} .*".format(copyright), line):
            break

    if line.lower().find("license") == -1:
        raise LegalIssue((
            "Line 3:\n"
            "\texpected license line\n"
            "\tfound    {}").format(repr(line)))

    lineno = 3
    while True:
        lineno += 1
        line = getnextline(it)
        if line == "EOF":
            raise LegalIssue((
                "Line {}:\n"
                "\texpected modifications copyright line\n"
                "\tfound    {}").format(lineno, repr(line)))

        match = testline("Modifications {}".format(oacopyright), line)
        if match:
            # we've found the "Modifications copyright line.
            # only one line remaining to check.
            if test_git_years:
                # let's see whether the years are actually correct
                test_git_first_last_year(filename,
                                         lineno,
                                         match.group(1),
                                         match.group(2))

            break

    if not getnextline(it) == "See copying.md for further legal info.":
        raise LegalIssue((
            "Line {}:\n"
            "\texpected copying.md reference line\n"
            "\tfound    {}").format(lineno + 1, repr(line)))

    return True
