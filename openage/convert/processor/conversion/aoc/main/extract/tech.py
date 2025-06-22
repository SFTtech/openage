# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Extract techs from the AoC data.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_tech import GenieTechObject

if typing.TYPE_CHECKING:
    from ......value_object.read.value_members import ArrayMember
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def extract_genie_techs(gamespec: ArrayMember, full_data_set: GenieObjectContainer) -> None:
    """
    Extract techs from the game data.

    :param gamespec: Gamedata from empires.dat file.
    :type gamespec: ...dataformat.value_members.ArrayMember
    """
    # Techs are stored as "researches"
    raw_techs = gamespec[0]["researches"].value

    index = 0
    for raw_tech in raw_techs:
        tech_id = index
        tech_members = raw_tech.value

        tech = GenieTechObject(tech_id, full_data_set, members=tech_members)
        full_data_set.genie_techs.update({tech.get_id(): tech})

        index += 1
