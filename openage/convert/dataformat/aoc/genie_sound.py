# Copyright 2019-2019 the openage authors. See copying.md for legal info.

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
        self.data.genie_sounds.update({self.get_id(): self})
