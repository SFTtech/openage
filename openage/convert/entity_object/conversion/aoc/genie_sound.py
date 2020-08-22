# Copyright 2019-2020 the openage authors. See copying.md for legal info.

"""
Contains structures and API-like objects for sounds from AoC.
"""

from ..converter_object import ConverterObject


class GenieSound(ConverterObject):
    """
    Sound definition from a .dat file.
    """

    __slots__ = ('data',)

    def __init__(self, sound_id, full_data_set, members=None):
        """
        Creates a new Genie sound object.

        :param sound_id: The sound id from the .dat file.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :param members: An already existing member dict.
        """

        super().__init__(sound_id, members=members)

        self.data = full_data_set

    def get_sounds(self, civ_id=-1):
        """
        Return sound resource ids for the associated DRS file.

        :param civ_id: If specified, only return sounds that belong to this civ.
        :type civ_id: int
        """
        sound_ids = []
        sound_items = self["sound_items"].get_value()
        for item in sound_items:
            item_civ_id = item["civilization_id"].get_value()
            if not item_civ_id == civ_id:
                continue

            sound_id = item["resource_id"].get_value()
            sound_ids.append(sound_id)

        return sound_ids

    def __repr__(self):
        return "GenieSound<%s>" % (self.get_id())
