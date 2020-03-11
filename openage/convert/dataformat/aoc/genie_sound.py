# Copyright 2019-2020 the openage authors. See copying.md for legal info.

from ...dataformat.converter_object import ConverterObject


class GenieSound(ConverterObject):
    """
    Sound definition from a .dat file.
    """

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

    def get_sounds_for_civ(self, civ_id):
        """
        Return all sound ids for sounds tied to a specific civ.
        """
        sound_ids = []
        sound_items = self.get_member("sound_items").get_value()
        for item in sound_items:
            item_civ_id = item.get_value()["civilization_id"].get_value()
            if not item_civ_id == civ_id:
                continue

            sound_id = item.get_value()["resource_id"].get_value()
            sound_ids.append(sound_id)

        return sound_ids

    def __repr__(self):
        return "GenieSound<%s>" % (self.get_id())
