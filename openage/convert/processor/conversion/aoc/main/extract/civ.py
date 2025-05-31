# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Extract civs from the AoC data.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_civ import GenieCivilizationObject

if typing.TYPE_CHECKING:
    from ......value_object.read.value_members import ArrayMember
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def extract_genie_civs(
    gamespec: ArrayMember,
    full_data_set: GenieObjectContainer
) -> None:
    """
    Extract civs from the game data.

    :param gamespec: Gamedata from empires.dat file.
    :type gamespec: class: ...dataformat.value_members.ArrayMember
    """
    raw_civs = gamespec[0]["civs"].value

    index = 0
    for raw_civ in raw_civs:
        civ_id = index

        civ_members = raw_civ.value
        units_member = civ_members.pop("units")
        units_member = units_member.get_container("id0")

        civ_members.update({"units": units_member})

        civ = GenieCivilizationObject(civ_id, full_data_set, members=civ_members)
        full_data_set.genie_civs.update({civ.get_id(): civ})

        index += 1
