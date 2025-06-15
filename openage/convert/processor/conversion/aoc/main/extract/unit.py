# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Extract units from the AoC data.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_unit import GenieUnitObject

if typing.TYPE_CHECKING:
    from ......value_object.read.value_members import ArrayMember
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def extract_genie_units(gamespec: ArrayMember, full_data_set: GenieObjectContainer) -> None:
    """
    Extract units from the game data.

    :param gamespec: Gamedata from empires.dat file.
    :type gamespec: ...dataformat.value_members.ArrayMember
    """
    # Units are stored in the civ container.
    # All civs point to the same units (?) except for Gaia which has more.
    # Gaia also seems to have the most units, so we only read from Gaia
    raw_units = gamespec[0]["civs"][0]["units"].value

    # Unit headers store the things units can do
    raw_unit_headers = gamespec[0]["unit_headers"].value

    for raw_unit in raw_units:
        unit_id = raw_unit["id0"].value
        unit_members = raw_unit.value

        # Turn attack and armor into containers to make diffing work
        if "attacks" in unit_members.keys():
            attacks_member = unit_members.pop("attacks")
            attacks_member = attacks_member.get_container("type_id")
            armors_member = unit_members.pop("armors")
            armors_member = armors_member.get_container("type_id")

            unit_members.update({"attacks": attacks_member})
            unit_members.update({"armors": armors_member})

        unit = GenieUnitObject(unit_id, full_data_set, members=unit_members)
        full_data_set.genie_units.update({unit.get_id(): unit})

        # Commands
        unit_commands = raw_unit_headers[unit_id]["unit_commands"]
        unit.add_member(unit_commands)
