# Copyright 2014-2022 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

"""
Dynamically load and unload data from a file at runtime.
"""

from __future__ import annotations
import typing

if typing.TYPE_CHECKING:
    from openage.convert.value_object.read.genie_structure import GenieStructure
    from openage.convert.value_object.init.game_version import GameVersion
    from openage.convert.value_object.read.value_members import ValueMember


class DynamicLoader:
    """
    Member that can be loaded and unloaded at runtime, saving
    memory in the process.
    """

    __slots__ = ('name', 'datacls', 'game_version', 'srcdata', 'offset', 'members', '_loaded')

    def __init__(
        self,
        name: str,
        datacls: type[GenieStructure],
        game_version: GameVersion,
        srcdata: bytes,
        offset: int
    ):
        self.datacls = datacls
        self.game_version = game_version

        self.srcdata = srcdata
        self.offset = offset
        self._loaded = False

        self.name = name
        self.members = None

    def load(self) -> dict[str, ValueMember]:
        """
        Read the members from the provided source data.
        """
        datacls = self.datacls()
        _, members = datacls.read(self.srcdata, self.offset, self.game_version, dynamic_load=False)

        self.members = {}
        for member in members:
            self.members[member.name] = member

        self._loaded = True

        return self.members

    def unload(self):
        """
        Delete the loaded members.
        """
        del self.members
        self._loaded = False

    def __getitem__(self, key) -> ValueMember:
        """
        Retrieve submembers from the loaded members or load them temporarily
        if they have not been loaded previously.
        """
        if self._loaded:
            return self.members[key]

        # else (expensive)
        self.load()
        item = self.members[key]
        self.unload()
        return item

    def __repr__(self) -> str:
        return f"DynamicLoader<{'loaded' if self._loaded else 'unloaded'}>"
