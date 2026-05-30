# Copyright 2015-2026 the openage authors. See copying.md for legal info.

"""
Checks some code style rules for cpp files.
"""

import re

from .util import findfiles, readfile, issue_str_line

# spaces missing in `if () {` and `for`, `while`, ...
MISSING_SPACES_RE = re.compile(
    # on of the folowing, the first group is used for the column where
    # the pointer is going to show
    r"(?:"
    # a ) folowed by a { without a space between
    r"(?:\)(\{))|"
    # a if/for/while folowed by a ( without a space between
    r"(?:\s+(?:if|for|while)(\())"
    r")"
)

# extra spaces before a ;
EXTRA_SPACES_RE = re.compile(
    # on of the folowing, the first group is used for the column where
    # the pointer is going to show
    r"(?:"
    # a space before a ";"
    r"(?:(\s+);)"
    r")"
)

# indentation fails looking at a file
INDENT_FAIL_RE = re.compile(
    # leading whitespace fail
    r"(?:"
    r"(\n\t*[ ]+\t+)"         # \n tab* space+ tab+
    r")"
)

# we need both because we look at the file first.
# indentation fails when looking at a line
INDENT_FAIL_LINE_RE = re.compile(
    # leading whitespace fail
    r"(?:"
    r"(^\t*[ ]+\t+)"          # tab* space+ tab+
    r")"
)

# Compound-assignment and comparison operators that must be surrounded by spaces.
# Ordered longest-first; < and >-based ops use lookbehind/lookahead to avoid
# matching the shorter pattern inside a longer one (e.g. >= inside >>=,
# <= inside <<= or the C++20 <=> spaceship operator).
_SPACED_OPS_ALT = (
    r'<<=|>>=|'
    r'\+=|-=|\*=|%=|\^=|\|=|&=|~=|'
    r'(?<!<)<=(?!>)|(?<!>)>=|'
    r'==|!='
)

OPERATOR_SPACING_RE = re.compile(
    r'(?<!\s)(?:' + _SPACED_OPS_ALT + r')'
    r'|'
    r'(?:' + _SPACED_OPS_ALT + r')(?!\s)'
)

# Strips // comments, inline /* */ comments, and double-quoted string literals
# by replacing each with equal-length spaces (preserves column offsets).
_CLEAN_LINE_RE = re.compile(r'//.*|/\*.*?\*/|"(?:[^"\\]|\\.)*"')

# Matches the start of a block-comment continuation line (e.g. " * text").
_BLOCK_CMT_CONT_RE = re.compile(r'^\s*\*')

# Matches the 'operator' keyword (used to skip operator-overload declarations).
_OPERATOR_KW_RE = re.compile(r'\boperator\b')


def filter_file_list(check_files, dirnames):
    """
    Yields all those files in check_files that are in one of the directories
    and end in '.cpp' or '.h' and some other conditions.
    """
    for filename in check_files:
        if not (filename.endswith('.cpp') or filename.endswith('.h')):
            continue

        if filename.endswith('.gen.h') or filename.endswith('.gen.cpp'):
            # TODO all this for now, until someone fixes the codegen.
            continue

        if any(filename.startswith(dirname) for dirname in dirnames):
            yield filename


def find_issues(check_files, dirnames):
    """
    Finds all issues in the given directories (filtered by check_files).
    """

    if check_files is not None:
        filenames = filter_file_list(check_files, dirnames)
    else:
        filenames = filter_file_list(findfiles(dirnames), dirnames)

    for filename in filenames:
        data = readfile(filename)
        analyse_each_line = False

        if MISSING_SPACES_RE.search(data) or\
           EXTRA_SPACES_RE.search(data) or\
           INDENT_FAIL_RE.search(data) or\
           OPERATOR_SPACING_RE.search(data):

            analyse_each_line = True

        # if there are possible issues perform a per line analysis
        if analyse_each_line:
            yield from find_issues_with_lines(data, filename)


def find_issues_with_lines(data, filename):
    """
    Checks a file for issues per line
    """

    for num, line in enumerate(data.splitlines(True), start=1):

        match = MISSING_SPACES_RE.search(line)
        if match:
            start = match.start(1) + match.start(2)
            end = start + 1
            yield issue_str_line("Missing space",
                                 filename, line, num,
                                 (start, end))

        match = EXTRA_SPACES_RE.search(line)
        if match:
            yield issue_str_line("Extra space",
                                 filename, line, num,
                                 (match.start(1), match.end(1)))

        match = INDENT_FAIL_LINE_RE.search(line)
        if match:
            yield issue_str_line("Wrong indentation",
                                 filename, line, num,
                                 (match.start(1), match.end(1)))

        # Check that compound-assignment and comparison operators have spaces
        # on both sides. Operate on a cleaned copy of the line (comments and
        # string literals blanked out) to avoid false positives; also skip
        # block-comment continuation lines and operator-overload declarations.
        check_line = _CLEAN_LINE_RE.sub(lambda m: ' ' * len(m.group()), line)
        if (not _BLOCK_CMT_CONT_RE.match(check_line) and
                not _OPERATOR_KW_RE.search(check_line)):
            match = OPERATOR_SPACING_RE.search(check_line)
            if match:
                yield issue_str_line("Missing space around operator",
                                     filename, line, num,
                                     (match.start(), match.end()))
