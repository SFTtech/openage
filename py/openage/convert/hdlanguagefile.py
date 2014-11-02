# Copyright 2014-2014 the openage authors. See copying.md for legal info.

from collections import defaultdict
from . import util
from .hardcoded.langcodes_hd import short_to_long_codes_map
from .util import dbg

class HDLanguageFile:
    """
    represents the translations text file found in the
    aoe:hd version (steam).
    """

    def __init__(self, fname, langcode):
        self.fname = fname
        self.lang = short_to_long_codes_map[langcode] if langcode in short_to_long_codes_map else langcode
        fname = util.file_get_path(fname)

        dbg("HD Language file [%s]" % (fname), 1)
        self.data = open(fname, mode='r', encoding='iso-8859-1').read()
        self.strings = self.extract_strings()

    def extract_strings(self):
        result = defaultdict(lambda: {})
        for i in self.data.split('\n'):
            i = i.strip()

            # skip comments & empty lines
            if i.startswith('//') or len(i) == 0:
                continue

            num, string = i.split(None, 1)

            # strings that were added in the HD edition release have UPPERCASE_STRINGS
            # as names, instead of the numeric ID stuff in AoK:TC
            # we only need the AoK:TC strings, and skip the rest
            if num.isdigit():
                result[self.lang][num] = string

        return result
