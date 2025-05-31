# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Extract terrains from the AoC data.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_terrain import GenieTerrainObject, \
    GenieTerrainRestriction

if typing.TYPE_CHECKING:
    from ......value_object.read.value_members import ArrayMember
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def extract_genie_terrains(gamespec: ArrayMember, full_data_set: GenieObjectContainer) -> None:
    """
    Extract terrains from the game data.

    :param gamespec: Gamedata from empires.dat file.
    :type gamespec: class: ...dataformat.value_members.ArrayMember
    """
    raw_terrains = gamespec[0]["terrains"].value

    for index, raw_terrain in enumerate(raw_terrains):
        terrain_index = index
        terrain_members = raw_terrain.value

        terrain = GenieTerrainObject(terrain_index, full_data_set, members=terrain_members)
        full_data_set.genie_terrains.update({terrain.get_id(): terrain})


def extract_genie_restrictions(
    gamespec: ArrayMember,
    full_data_set: GenieObjectContainer
) -> None:
    """
    Extract terrain restrictions from the game data.

    :param gamespec: Gamedata from empires.dat file.
    :type gamespec: class: ...dataformat.value_members.ArrayMember
    """
    raw_restrictions = gamespec[0]["terrain_restrictions"].value

    for index, raw_restriction in enumerate(raw_restrictions):
        restriction_index = index
        restriction_members = raw_restriction.value

        restriction = GenieTerrainRestriction(restriction_index,
                                              full_data_set,
                                              members=restriction_members)
        full_data_set.genie_terrain_restrictions.update({restriction.get_id(): restriction})
