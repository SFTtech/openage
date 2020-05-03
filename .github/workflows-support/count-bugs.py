#!/usr/bin/python3

# Copyright (c) 2019 Patryk Obara <patryk.obara@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

"""
This script prints a summary snippet of information out of reports created
by scan-build or analyze-build for Clang's static code analysis.

This script counts the number of reported issues and compares that
against provided maximum. if the count exceeds the maximum then the
script will return a status of 1 (failure), otherwise the script
returns 0 (success).

If you prefer to not set a maximum, then supply a negative maximum
such as -1.  If a maximum is not provided on the command line then
the internally-defined maximum is used.

Usage: ./count-warnings.py [max-issues] path/to/report/index.html

Examples:
 use internal maximum: ./count-bugs.py path/to/report/index.html
 permit any quantity:  ./count-bugs.py -1 path/to/report/index.html
 limit count to 48:    ./count-bugs.py 48 path/to/report/index.html

"""
# This script depends on BeautifulSoup module, if you're distribution is
# missing it, you can use pipenv to install it for virtualenv spanning only
# this repo: pipenv install beautifulsoup4 html5lib

# pylint: disable=invalid-name
# pylint: disable=missing-docstring

import os

from bs4 import BeautifulSoup

# Maximum allowed number of issues. This is used if the user does
# not prove a maximum via the first command-line arugment.
#
MAX_ISSUES = 40


def summary_values(summary_table):
    if not summary_table:
        return
    for row in summary_table.find_all('tr'):
        description = row.find('td', 'SUMM_DESC')
        value = row.find('td', 'Q')
        if description is None or value is None:
            continue
        yield description.get_text(), int(value.get_text())


def read_soup(index_html):
    with open(index_html) as index:
        soup = BeautifulSoup(index, 'html5lib')
        tables = soup.find_all('table')
        summary = tables[1]
        return dict(summary_values(summary))


def find_longest_name_length(names):
    return max(len(x) for x in names)


def print_summary(issues):
    summary = list(issues.items())
    size = find_longest_name_length(issues.keys()) + 1
    for warning, count in sorted(summary, key=lambda x: -x[1]):
        print('  {text:{field_size}s}: {count}'.format(
            text=warning, count=count, field_size=size))
    print()


def parse_args(argv):
    # Guard: show the usage block for incorrect usage
    if len(argv) not in [2, 3]:
        print(__doc__)
        sys.exit(1)
    # Guard: check if the provided report file exists
    report = argv[-1]
    if not os.path.isfile(report):
        print("{}: no such file".format(report))
        sys.exit(1)
    # Good scenario: set our maximum and return it with the log
    max_issues = MAX_ISSUES if len(argv) == 2 else int(argv[1])
    return max_issues, report


def main(argv):
    rcode = 0
    max_issues, report = parse_args(argv)
    bug_types = read_soup(report)
    total = bug_types.pop('All Bugs')
    if bug_types:
        print("Bugs grouped by type:\n")
        print_summary(bug_types)

    print('Total: {} bugs'.format(total), end='')
    if max_issues >= 0:
        print(' (out of {} allowed)\n'.format(max_issues))
        if total > max_issues:
            print('Error: upper limit of allowed bugs is', max_issues)
            rcode = 1
    else:
        print('\n')

    return rcode


if __name__ == '__main__':
    import sys

    sys.exit(main(sys.argv))
