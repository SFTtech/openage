# Copyright 2014-2015 the openage authors. See copying.md for legal info.

from collections import defaultdict
from .hardcoded.langcodes_hd import short_to_long_codes_map
from .util import dbg


class HDLanguageFile:
    """
    represents the translations text file found in the
    aoe:hd version (steam).
    """

    def __init__(self, fname, langcode):
        self.fname = fname
        if langcode in short_to_long_codes_map:
            self.lang = short_to_long_codes_map[langcode]
        else:
            self.lang = langcode

        dbg("HD Language file [%s]" % (fname), 1)
        self.strings = defaultdict(lambda: dict())

        with open(fname, mode='r', encoding='iso-8859-1') as f:
            data = f.read()
            for i in data.split('\n'):
                i = i.strip()

                # skip comments & empty lines
                if not i or i.startswith('//'):
                    continue

                num, string = i.split(None, 1)

                # strings that were added in the HD edition release have
                # UPPERCASE_STRINGS as names, instead of the numeric ID stuff
                # in AoK:TC. We only need the AoK:TC strings, and skip the rest.
                if num.isdigit():
                    self.strings[self.lang][num] = string
