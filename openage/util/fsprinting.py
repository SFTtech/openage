# Copyright 2015-2016 the openage authors. See copying.md for legal info.

"""
Methods for printing paths and other file system-related info.
"""

from collections import OrderedDict

from .strings import colorize
from .math import INF

RULE_CACHE = OrderedDict()


def get_color_rules():
    """
    Returns a dict of pattern : colorcode, retrieved from LS_COLORS.
    """
    if RULE_CACHE:
        return RULE_CACHE

    from os import environ

    try:
        rules = environ['LS_COLORS']
    except KeyError:
        return {}

    for rule in rules.split(':'):
        rule = rule.strip()
        if not rule:
            continue

        try:
            pattern, colorcode = rule.split('=', maxsplit=1)
        except ValueError:
            # Your LS_COLORS are broken. Go fix them.
            # I shouldn't even be catching the error for you.
            continue

        RULE_CACHE[pattern] = colorcode

    return RULE_CACHE


def colorize_filename(filename):
    """
    Colorizes the filename, using the globbing rules from LS_COLORS.
    """
    from fnmatch import fnmatch

    rules = get_color_rules()

    for pattern, colorcode in rules.items():
        if fnmatch(filename, pattern):
            return colorize(filename, colorcode)

    return colorize(filename, rules.get('fi'))


def colorize_dirname(dirname):
    """
    Colorizes the dirname, using the 'di' rule from LS_COLORS.
    """
    return colorize(dirname, get_color_rules().get('di'))


def print_tree(obj, path="", prefix="", max_entries=INF):
    """
    Obj is a filesystem-like object; path must be a string.

    Recursively descends into subdirectories using prefix.

    If max_entries is given, only that number of entries per directory
    is printed.
    """
    # entries is a list of tuples of entryname, isdir, ismeta
    entries = []
    for entry in obj.listdirs(path):
        entries.append((entry, True, False))
    for entry in obj.listfiles(path):
        entries.append((entry, False, False))

    if not entries:
        entries.append(("[empty]", False, True))

    if len(entries) > max_entries:
        omit = len(entries) - max_entries + 1
        entries = entries[:-omit] + [("[%d omitted]" % omit, False, True)]

    from .iterators import denote_last
    for (name, isdir, is_meta), is_last in denote_last(entries):
        if is_last:
            treesymbol, nextindent = '\u2514', '   '
        else:
            treesymbol, nextindent = '\u251c', '\u2502  '

        if is_meta:
            entryindent = '\u257c '
        else:
            entryindent = '\u2500 '

        if isdir:
            print(prefix + treesymbol + entryindent + colorize_dirname(name))
            obj.print_tree(path + '/' + name, prefix + nextindent, max_entries)
            print(prefix + nextindent)
        else:
            print(prefix + treesymbol + entryindent + colorize_filename(name))
