# Copyright 2014-2020 the openage authors. See copying.md for legal info.

"""
Module for reading AoeII HD Edition text-based language files.
"""

from openage.convert.langfile.langcodes import LANGCODES_DE2

from ...log import dbg
from .langcodes import LANGCODES_HD
from .pefile import PEFile


def read_age2_hd_fe_stringresources(stringres, path):
    """
    Fill the string resources from text specifications found
    in the given path.

    In age2hd forgotten those are stored in plain text files.

    The data is stored in the `stringres` storage.
    """

    count = 0

    # multiple string files in the program source dir
    for lang in path.list():
        try:
            if lang == b'_common':
                continue
            if lang == b'_packages':
                continue
            if lang.lower() == b'.ds_store'.lower():
                continue

            langfilename = [lang.decode(),
                            "strings", "key-value",
                            "key-value-strings-utf8.txt"]

            with path[langfilename].open('rb') as langfile:
                stringres.fill_from(read_hd_language_file(langfile, lang))

            count += 1

        except FileNotFoundError:
            # that's fine, there are no language files for every language.
            pass

    return count


def read_age2_hd_3x_stringresources(stringres, srcdir):
    """
    HD Edition 3.x and below store language .txt files
    in the Bin/ folder.
    Specific language strings are in Bin/$LANG/*.txt.

    The data is stored in the `stringres` storage.
    """

    count = 0

    for lang in srcdir["bin"].list():
        lang_path = srcdir["bin", lang.decode()]

        # There are some .txt files immediately in bin/, but they don't
        # seem to contain anything useful. (Everything is overridden by
        # files in Bin/$LANG/.)
        if not lang_path.is_dir():
            continue

        # Sometimes we can have language DLLs in Bin/$LANG/
        # e.g. HD Edition 2.0
        # We do _not_ want to treat these as text files
        # so first check explicitly

        if lang_path["language.dll"].is_file():
            for name in ["language.dll",
                         "language_x1.dll",
                         "language_x1_p1.dll"]:

                pefile = PEFile(lang_path[name].open('rb'))
                stringres.fill_from(pefile.resources().strings)
                count += 1

        else:
            for basename in lang_path.list():
                with lang_path[basename].open('rb') as langfile:
                    # No utf-8 :(
                    stringres.fill_from(
                        read_hd_language_file(
                            langfile, lang, enc='iso-8859-1'))
                count += 1

    return count


def read_hd_language_file(fileobj, langcode, enc='utf-8'):
    """
    Takes a file object, and the file's language code.
    """

    dbg("parse HD Language file %s", langcode)
    strings = {}

    for line in fileobj.read().decode(enc).split('\n'):
        line = line.strip()

        # skip comments & empty lines
        if not line or line.startswith('//'):
            continue

        string_id, string = line.split(None, 1)

        # strings that were added in the HD edition release have
        # UPPERCASE_STRINGS as names, instead of the numeric ID stuff
        # of AoC.
        strings[string_id] = string

    fileobj.close()

    lang = LANGCODES_HD.get(langcode, langcode)

    return {lang: strings}


def read_de2_language_file(srcdir, language_file):
    """
    Definitve Edition stores language .txt files in the resources/ folder.
    Specific language strings are in resources/$LANG/strings/key-value/*.txt.

    The data is stored in the `stringres` storage.
    """
    # Langcode is folder name
    langcode = language_file.split("/")[1]

    dbg("parse DE2 Language file %s", langcode)
    strings = {}

    fileobj = srcdir[language_file].open('rb')

    for line in fileobj.read().decode('utf-8').split('\n'):
        line = line.strip()

        # skip comments & empty lines
        if not line or line.startswith('//'):
            continue

        string_id, string = line.split(None, 1)

        # strings that were added in the HD edition release have
        # UPPERCASE_STRINGS as names, instead of the numeric ID stuff
        # of AoC.
        strings[string_id] = string

    fileobj.close()

    lang = LANGCODES_DE2.get(langcode, langcode)

    return {lang: strings}
