# Copyright 2014-2014 the openage authors. See copying.md for legal info.

import shutil
import os


def depend_error(text):
    print("\x1b[1;33mWARNING:\x1b[m unmet runtime dependency: {}".format(text))


def depend_py_module(name):
    namespace = {}

    try:
        exec("import " + name)
    except ImportError:
        depend_error("python module: " + name)


def depend_utility(name):
    if not shutil.which(name):
        depend_error("utility program: " + name)


def depend_file(name):
    if not os.path.isfile(name):
        depend_error("file: " + name)


def warn():
    # a list of imported modules can be obtained via
    #
    # grep -RE '^ *(import |from [^.])' | cut -d: -f2- | \
    #         sed 's/^ *//g' | sort -u | grep -v openage

    depend_py_module("PIL.Image")
    depend_py_module("PIL.ImageDraw")
    depend_utility("opusenc")


if __name__ == '__main__':
    warn()
