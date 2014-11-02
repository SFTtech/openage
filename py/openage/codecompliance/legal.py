# Copyright 2014-2014 the openage authors. See copying.md for legal info.

import os
import itertools
import re
from subprocess import Popen, PIPE

from . import util

nativelegalheader = re.compile(
    "^"
    # allow shebang line, followed by an optional empty line
    "(#!/.*\n(#?\n)?)?"

    # next line must be the copyright line
    "(#|//) "
    "Copyright (?P<crstart>\\d{4})-(?P<crend>\\d{4}) the openage authors\\. "
    "See copying\\.md for legal info\\.\n")

thirdpartylegalheader = re.compile(
    "^"
    # 3rd-party copyright/license
    "(#|//) This file (was (taken|adapted)|contains (data|code)) from .*\n"
    "(#|//) Copyright \\d{4}-\\d{4} .*\n"
    "(#|//) .*license.*\n"

    # any number of lines containing further 3rd-party copyright info
    "((#|//) .*\\n)*"

    # the openage copyright
    "(#|//) (Modifications|Other (data|code)|Everything else) "
    "Copyright (?P<crstart>\\d{4})-(?P<crend>\\d{4}) the openage authors\\.\n"
    "(#|//) See copying\\.md for further legal info\\.\n")

# empty files (consisting of only comments) don't require a legal header
emptyfile = re.compile("^(((#|//) .*)?\n)*$")


class LegalHeaderMatch:
    def __init__(self, legalheader, match, data):
        self.legalheader = legalheader
        self.match = match
        self.data = data

    def years(self):
        """
        returns a tuple startyear, stopyear of the copyright definition
        """
        try:
            return self.match.group("crstart"), self.match.group("crend")
        except:
            return None

    def fullheader(self):
        """
        returns the full legal header of the file
        """
        return self.match.group(0)

    def remainder(self):
        """
        returns the rest (non-legal-header part) of the file
        """
        return self.data[len(self.match.group(0)):]


extensions_requiring_legal_headers = {
    '.h', '.cpp', '.py', ".cmake",
    '.h.in', '.cpp.in', '.py.in',
    '.h.template', '.cpp.template', '.py.template',
}


def match_legalheader(data):
    for hdr in (nativelegalheader, thirdpartylegalheader, emptyfile):
        m = re.match(hdr, data)
        if m is not None:
            return hdr, m

    raise LegalIssue("No valid legal header text found")


class LegalIssue(Exception):
    pass


def find_issues(paths, git_copyright_timespan=False):
    """
    tests all source files for the required legal headers.
    """
    third_party_files = set()

    # test all in-sourcefile legal headers
    for filename in util.findfiles(paths, extensions_requiring_legal_headers):
        try:
            headertype, match = match_legalheader(util.readfile(filename))

            if git_copyright_timespan:
                try:
                    start_year = match.group('crstart')
                    end_year = match.group('crend')
                except IndexError:
                    # the header doesn't contain copyright years; don't check
                    # them (probably it's an empty file)
                    pass
                else:
                    test_git_copyright_timespan(filename, start_year, end_year)

            if headertype is thirdpartylegalheader:
                third_party_files.add(filename)

        except LegalIssue as e:
            yield util.Issue(
                "legal header text issue in {}".format(filename),
                "\n\t{}\n\tSee copying.md for guidelines and a "
                "template".format(e.args[0]))

    # test whether all third-party files are listed in copying.md
    listed_files = set()
    for line in util.readfile('copying.md').split('\n'):
        match = re.match("^ - `([^`]+)`.*$", line)
        if not match:
            continue

        filename = match.group(1)
        listed_files.add(filename)

    # file listed, but has no 3rd-party header?
    for filename in sorted(listed_files - third_party_files):
        if util.has_ext(filename, extensions_requiring_legal_headers):
            yield util.Issue(
                "third-party file listing issue",
                ("{}\n\tlisted in copying.md, but has no "
                 "third-party license header.").format(filename))

    # file has 3rd-party header, but is not listed?
    for filename in sorted(third_party_files - listed_files):
        yield util.Issue("third-party file listing issue",
                         ("{}\n\thas a third-party license header, but isn't "
                          "listed in copying.md").format(filename))


def test_git_copyright_timespan(filename, hdr_start_year, hdr_stop_year):

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

    if hdr_start_year != output[-1][:4]:
        raise LegalIssue((
            "\tBad copyright start year in legal header:\n"
            "\texpected {} (from git history start {})\n"
            "\tfound    {}").format(
                output[-1][:4], output[-1],
                hdr_start_year))

    if hdr_stop_year != output[0][:4]:
        raise LegalIssue((
            "\tBad copyright stop year in legal header:\n"
            "\texpected {} (from git history end {})\n"
            "\tfound    {}").format(
                output[0][:4], output[0],
                hdr_stop_year))
