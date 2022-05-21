# Copyright 2014-2022 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,too-many-function-args

from __future__ import annotations
import typing

from collections import defaultdict

from ...value_object.read.genie_structure import GenieStructure

if typing.TYPE_CHECKING:
    from openage.convert.value_object.init.game_version import GameVersion


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
    def get_data_format_members(cls, game_version: GameVersion) -> tuple:
        """
        Return the members in this struct.
        """
        data_format = (
            (True, "id", None,   "int32_t"),
            (True, "lang", None, "char[16]"),
            (True, "text", None, "std::string"),
        )

        return data_format
