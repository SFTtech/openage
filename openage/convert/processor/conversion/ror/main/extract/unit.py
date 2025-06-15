# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Extract units from the RoR data.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_unit import GenieUnitObject

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer
    from ......value_object.read.value_members import ArrayMember


def extract_genie_units(gamespec: ArrayMember, full_data_set: GenieObjectContainer) -> None:
    """
    Extract units from the game data.

    :param gamespec: Gamedata from empires.dat file.
    """
    # Units are stored in the civ container.
    # In RoR the normal civs are not subsets of the Gaia civ, so we search units from
    # Gaia and one player civ (egyptiians).
    raw_units = []

    # Gaia units
    raw_units.extend(gamespec[0]["civs"][0]["units"].value)

    # Egyptians
    raw_units.extend(gamespec[0]["civs"][1]["units"].value)

    for raw_unit in raw_units:
        unit_id = raw_unit["id0"].value

        if unit_id in full_data_set.genie_units.keys():
            continue

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

    # Sort the dict to make debugging easier :)
    full_data_set.genie_units = dict(sorted(full_data_set.genie_units.items()))
