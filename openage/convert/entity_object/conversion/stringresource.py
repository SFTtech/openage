# Copyright 2014-2021 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,too-many-function-args

from collections import defaultdict

from ...entity_object.conversion.genie_structure import GenieStructure


class StringResource(GenieStructure):

    def __init__(self):
        super().__init__()
        self.strings = defaultdict(lambda: {})

    def fill_from(self, stringtable: dict[str, dict[str, str]]) -> None:
        """
        stringtable is a dict {langcode: {id: string}}
        """
        for lang, langstrings in stringtable.items():
            self.strings[lang].update(langstrings)

    def get_tables(self) -> dict[str, dict[str, str]]:
        """
        Returns the stringtable.
        """
        return self.strings

    @classmethod
    def get_data_format_members(cls, game_version: tuple) -> tuple:
        """
        Return the members in this struct.
        """
        data_format = (
            (True, "id", None,   "int32_t"),
            (True, "lang", None, "char[16]"),
            (True, "text", None, "std::string"),
        )

        return data_format
