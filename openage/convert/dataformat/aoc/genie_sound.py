# Copyright 2019-2019 the openage authors. See copying.md for legal info.

from bin.openage.convert.dataformat.converter_object import ConverterObject


class GenieSound(ConverterObject):
    """
    Sound definition from a .dat file.
    """

    def __init__(self, sound_id, full_data_set):
        """
        Creates a new Genie sound object.

        :param sound_id: The sound id from the .dat file.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(sound_id)

        self.data = full_data_set
        self.data.genie_sounds.update({self.get_id(): self})
