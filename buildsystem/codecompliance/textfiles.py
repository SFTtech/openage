# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Checks some general whitespace rules and the encoding for text files.
"""

from .util import findfiles, readfile, has_ext, BADUTF8FILES


def find_issues(dirnames, exts):
    """
    checks all files ending in exts in dirnames.
    """
    for filename in findfiles(dirnames, exts):
        data = readfile(filename)

        if ' \n' in data or '\t\n' in data:
            if filename.startswith('openage/') and filename.endswith('.cpp'):
                # allow this particular error for Cython-generated files.
                pass
            elif filename.endswith('.gen.h') or filename.endswith('.gen.cpp'):
                # TODO all this for now, until someone fixes the codegen.
                pass
            else:
                # TODO calculate the line number
                yield "Trailing whitespace", filename

        if data.endswith('\n\n') or data.endswith('\n\r\n'):
            yield "Trailing newline", filename

        if len(data) > 0 and not data.endswith('\n'):
            yield "File does not end in '\\n'", filename

        if has_ext(filename, ('.py', '.pyx', '.pxd')):
            if '\t' in data:
                yield "File contains tabs", filename

    for filename in BADUTF8FILES:
        yield "Not valid UTF-8", filename
