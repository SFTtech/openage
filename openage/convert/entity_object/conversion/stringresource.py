# Copyright 2014-2021 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,too-many-function-args

from collections import defaultdict

from ...entity_object.conversion.genie_structure import GenieStructure
from ...entity_object.export import data_definition


class StringResource(GenieStructure):

    def __init__(self):
        super().__init__()
        self.strings = defaultdict(lambda: {})

    def fill_from(self, stringtable):
        """
        stringtable is a dict {langcode: {id: string}}
        """
        for lang, langstrings in stringtable.items():
            self.strings[lang].update(langstrings)

    def get_tables(self):
        """
        Returns the stringtable.
        """
        return self.strings

    def dump(self, filename):
        data = []

        for lang, langstrings in self.strings.items():
            for idx, text in langstrings.items():
                entry = {
                    "id": idx,
                    "lang": lang,
                    "text": text,
                }
                data.append(entry)

        data = sorted(data, key=lambda x: x["id"])
        return [data_definition.DataDefinition(self, data, filename)]

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = (
            (True, "id", None,   "int32_t"),
            (True, "lang", None, "char[16]"),
            (True, "text", None, "std::string"),
        )

        return data_format
