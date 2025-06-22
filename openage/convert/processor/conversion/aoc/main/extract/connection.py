# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Extract connection objects from the AoC data.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_connection import GenieAgeConnection, \
    GenieBuildingConnection, GenieUnitConnection, GenieTechConnection

if typing.TYPE_CHECKING:
    from ......value_object.read.value_members import ArrayMember
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def extract_age_connections(gamespec: ArrayMember, full_data_set: GenieObjectContainer) -> None:
    """
    Extract age connections from the game data.

    :param gamespec: Gamedata from empires.dat file.
    :type gamespec: class: ...dataformat.value_members.ArrayMember
    """
    raw_connections = gamespec[0]["age_connections"].value

    for raw_connection in raw_connections:
        age_id = raw_connection["id"].value
        connection_members = raw_connection.value

        connection = GenieAgeConnection(age_id, full_data_set, members=connection_members)
        full_data_set.age_connections.update({connection.get_id(): connection})


@staticmethod
def extract_building_connections(
    gamespec: ArrayMember,
    full_data_set: GenieObjectContainer
) -> None:
    """
    Extract building connections from the game data.

    :param gamespec: Gamedata from empires.dat file.
    :type gamespec: class: ...dataformat.value_members.ArrayMember
    """
    raw_connections = gamespec[0]["building_connections"].value

    for raw_connection in raw_connections:
        building_id = raw_connection["id"].value
        connection_members = raw_connection.value

        connection = GenieBuildingConnection(building_id, full_data_set,
                                             members=connection_members)
        full_data_set.building_connections.update({connection.get_id(): connection})


@staticmethod
def extract_unit_connections(
    gamespec: ArrayMember,
    full_data_set: GenieObjectContainer
) -> None:
    """
    Extract unit connections from the game data.

    :param gamespec: Gamedata from empires.dat file.
    :type gamespec: class: ...dataformat.value_members.ArrayMember
    """
    raw_connections = gamespec[0]["unit_connections"].value

    for raw_connection in raw_connections:
        unit_id = raw_connection["id"].value
        connection_members = raw_connection.value

        connection = GenieUnitConnection(unit_id, full_data_set, members=connection_members)
        full_data_set.unit_connections.update({connection.get_id(): connection})


@staticmethod
def extract_tech_connections(
    gamespec: ArrayMember,
    full_data_set: GenieObjectContainer
) -> None:
    """
    Extract tech connections from the game data.

    :param gamespec: Gamedata from empires.dat file.
    :type gamespec: class: ...dataformat.value_members.ArrayMember
    """
    raw_connections = gamespec[0]["tech_connections"].value

    for raw_connection in raw_connections:
        tech_id = raw_connection["id"].value
        connection_members = raw_connection.value

        connection = GenieTechConnection(tech_id, full_data_set, members=connection_members)
        full_data_set.tech_connections.update({connection.get_id(): connection})
