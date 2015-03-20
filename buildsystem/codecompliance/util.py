# Copyright 2014-2015 the openage authors. See copying.md for legal info.

"""
Some utilities.
"""

import os


FILECACHE = {}
BADUTF8FILES = set()


def has_ext(fname, exts):
    """
    Returns true if fname ends in any of the extensions in ext.
    """
    for ext in exts:
        if ext == '':
            if os.path.splitext(fname)[1] == '':
                return True
        elif fname.endswith(ext):
            return True

    return False


def readfile(filename):
    """
    reads the file, and returns it as a str object.

    if the file has already been read in the past,
    returns it from the cache.
    """
    if filename not in FILECACHE:
        with open(filename, 'rb') as fileobj:
            data = fileobj.read()

        try:
            data = data.decode('utf-8')
        except UnicodeDecodeError:
            data = data.decode('utf-8', errors='replace')
            BADUTF8FILES.add(filename)

        FILECACHE[filename] = data

    return FILECACHE[filename]


def findfiles(paths, exts):
    """
    yields all files in paths with names ending in an ext from exts.

    hidden dirs and files are ignored.
    """
    for path in paths:
        for filename in os.listdir(path):
            if filename.startswith('.'):
                continue

            filename = os.path.join(path, filename)

            if os.path.isdir(filename):
                yield from findfiles((filename,), exts)
                continue

            if has_ext(filename, exts):
                yield filename
