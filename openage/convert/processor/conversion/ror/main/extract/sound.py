# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Extract sounds from the RoR data.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.ror.genie_sound import RoRSound

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer
    from ......value_object.read.value_members import ArrayMember


def extract_genie_sounds(gamespec: ArrayMember, full_data_set: GenieObjectContainer) -> None:
    """
    Extract sound definitions from the game data.

    :param gamespec: Gamedata from empires.dat file.
    """
    # call hierarchy: wrapper[0]->sounds
    raw_sounds = gamespec[0]["sounds"].value
    for raw_sound in raw_sounds:
        sound_id = raw_sound["sound_id"].value
        sound_members = raw_sound.value

        sound = RoRSound(sound_id, full_data_set, members=sound_members)
        full_data_set.genie_sounds.update({sound.get_id(): sound})
