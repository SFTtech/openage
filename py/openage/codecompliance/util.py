# Copyright 2014-2014 the openage authors. See copying.md for legal info.

import os
from collections import defaultdict


class Issue:
    def __init__(self, title, text):
        self.title = title
        self.text = text

    def print(self):
        if os.isatty(1):
            print("\x1b[33;1mWARNING\x1b[m {}: {}".format(
                self.title,
                self.text))
        else:
            print("WARNING {}: {}".format(self.title, self.text))


filecache = {}
badutf8files = set()


def readfile(filename):
    """
    reads the file, and returns it as a str object.

    if the file has already been read in the past,
    returns it from the cache.
    """
    if filename not in filecache:
        with open(filename, 'rb') as f:
            data = f.read()

        try:
            data = data.decode('utf-8')
        except:
            data = data.decode('utf-8', errors='replace')
            badutf8files.add(filename)

        filecache[filename] = data

    return filecache[filename]


def has_ext(fname, exts):
    for ext in exts:
        if fname.endswith(ext):
            return True

    return False


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
                for f in findfiles((filename,), exts):
                    yield f

            if has_ext(filename, exts):
                yield filename
