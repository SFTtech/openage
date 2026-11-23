# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Checks some code style rules for cpp files.
"""

import re

from .util import issue_str_line, readfile, select_files

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
    r"(\n\t*[ ]+\t+)"  # \n tab* space+ tab+
    r")"
)

# C++ module import and include placement
IMPORT_RE = re.compile(r"^\s*(?:export\s+)?import\s+[\w.:<>\"/]+\s*;")
MODULE_DECL_RE = re.compile(r"^\s*(?:export\s+)?module\s+[\w.:]+\s*;")
INCLUDE_RE = re.compile(r"^\s*#\s*include\b")
COMMENT_RE = re.compile(r"/\*.*?\*/|//[^\n]*", re.DOTALL)

# we need both because we look at the file first.
# indentation fails when looking at a line
INDENT_FAIL_LINE_RE = re.compile(
    # leading whitespace fail
    r"(?:"
    r"(^\t*[ ]+\t+)"  # tab* space+ tab+
    r")"
)


def filter_file_list(check_files, dirnames):
    """
    Yields all those files in check_files that are in one of the directories
    and end in '.cpp', '.cppm' or '.h' and some other conditions.
    """
    for filename in select_files(check_files, dirnames, (".cpp", ".cppm", ".h")):
        if filename.name.endswith((".gen.h", ".gen.cpp")):
            # TODO all this for now, until someone fixes the codegen.
            continue

        yield filename


def find_issues(check_files, dirnames):
    """
    Finds all issues in the given directories (filtered by check_files).
    """

    filenames = filter_file_list(check_files, dirnames)

    for filename in filenames:
        data = readfile(filename)
        analyse_each_line = False

        if MISSING_SPACES_RE.search(data) or EXTRA_SPACES_RE.search(data) or INDENT_FAIL_RE.search(data):
            analyse_each_line = True

        # if there are possible issues perform a per line analysis
        if analyse_each_line:
            yield from find_issues_with_lines(data, filename)

        yield from find_module_issues(data, filename)


def find_module_issues(data, filename):
    """
    Checks that imports come after all includes.

    An include after the module declaration would attach the header to the
    module. GCC rejects textual definitions after an import of the same
    entities; the build exempts only the standard headers by translating
    them into a header unit, see buildsystem/StdHeaderUnit.cmake.
    """
    # blank out the comments but keep the line numbers
    code = COMMENT_RE.sub(lambda match: re.sub(r"[^\n]", " ", match.group()), data)
    seen = None

    for num, line in enumerate(code.splitlines(True), start=1):
        match = IMPORT_RE.match(line)
        if match:
            seen = seen or "import"
            continue

        match = MODULE_DECL_RE.match(line)
        if match:
            seen = seen or "module declaration"
            continue

        match = INCLUDE_RE.match(line)
        if match and seen:
            yield issue_str_line(f"Include after {seen}", filename, line, num, match.span())


def find_issues_with_lines(data, filename):
    """
    Checks a file for issues per line
    """

    for num, line in enumerate(data.splitlines(True), start=1):
        match = MISSING_SPACES_RE.search(line)
        if match:
            start = match.start(1) + match.start(2)
            end = start + 1
            yield issue_str_line("Missing space", filename, line, num, (start, end))

        match = EXTRA_SPACES_RE.search(line)
        if match:
            yield issue_str_line("Extra space", filename, line, num, (match.start(1), match.end(1)))

        match = INDENT_FAIL_LINE_RE.search(line)
        if match:
            yield issue_str_line("Wrong indentation", filename, line, num, (match.start(1), match.end(1)))
