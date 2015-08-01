# Copyright 2014-2015 the openage authors. See copying.md for legal info.

"""
Module for reading AoeII HD Edition text-based language files.
"""

from .hardcoded.langcodes_hd import LANGCODE_MAP_HD
from ..log import dbg


def read_hd_language_file(fileobj, langcode):
    """
    Takes a file object, and the file's language code.
    """
    dbg("HD Language file " + str(langcode))
    strings = {}

    for line in fileobj.read().decode('iso-8859-1').split('\n'):
        line = line.strip()

        # skip comments & empty lines
        if not line or line.startswith('//'):
            continue

        num, string = line.split(None, 1)

        # strings that were added in the HD edition release have
        # UPPERCASE_STRINGS as names, instead of the numeric ID stuff
        # of AoK:TC. We only need the AoK:TC strings, and skip the rest.
        if num.isdigit():
            strings[num] = string

    fileobj.close()

    lang = LANGCODE_MAP_HD.get(langcode, langcode)

    return {lang: strings}
