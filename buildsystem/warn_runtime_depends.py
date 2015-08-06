# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Attempts to import the known third-party runtime dependencies,
and prints warnings on failure.
"""

# command-line tools

UTILITIES = [
    "opusenc"
]

# python modules
# a list of imported modules may be obtained via
#
# grep -RE '^ *(import |from [^.])' | cut -d: -f2- | \
#     sed 's/^ *//g' | sort -u | grep -v openage

MODULES = [
    "PIL.Image",
    "PIL.ImageDraw",
    "numpy"
]


def depend_error(text):
    """
    prints a nice and colorful warning message.
    """
    print("\x1b[1;33mWARNING:\x1b[m unmet runtime dependency: {}".format(text))


def main():
    """ tests for all the modules and utilities defined above """
    from shutil import which
    from importlib import import_module

    for module in MODULES:
        try:
            import_module(module)
        except ImportError:
            depend_error("python module: " + module)

    for utility in UTILITIES:
        if not which(utility):
            depend_error("utility program: " + utility)


if __name__ == '__main__':
    main()
