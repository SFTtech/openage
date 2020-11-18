# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Contains structures and API-like objects for sounds from RoR.

Based on the classes from the AoC converter.
"""

from ..aoc.genie_sound import GenieSound


class RoRSound(GenieSound):
    """
    Sound definition from a .dat file. Some methods are reimplemented as RoR does not
    have all features from AoE2.
    """

    def get_sounds(self, civ_id=-1):
        """
        Does not search for the civ id because RoR does not have
        a .dat entry for that.
        """
        sound_ids = []
        sound_items = self["sound_items"].get_value()
        for item in sound_items:
            sound_id = item["resource_id"].get_value()
            sound_ids.append(sound_id)

        return sound_ids

    def __repr__(self):
        return f"RoRSouns<{self.get_id()}>"
