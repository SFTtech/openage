# Copyright 2014-2020 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,too-many-function-args
# REFA: Whole package -> REMOVE

from collections import defaultdict

from ..dataformat.genie_structure import GenieStructure
from ..export import data_definition, struct_definition


class StringResource(GenieStructure):
    name_struct = "string_resource"
    name_struct_file = "string_resource"
    struct_description = "string id/language to text mapping,"\
                         " extracted from language.dll file."

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
        data = list()

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
    def structs(cls):
        return [struct_definition.StructDefinition(cls)]

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
