# Copyright 2019-2019 the openage authors. See copying.md for legal info.

from bin.openage.convert.dataformat.converter_object import ConverterObject


class GenieGraphic(ConverterObject):
    """
    Graphic definition from a .dat file.
    """

    def __init__(self, graphic_id, full_data_set):
        """
        Creates a new Genie graphic object.

        :param graphic_id: The graphic id from the .dat file.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(graphic_id)

        self.data = full_data_set
        self.data.genie_graphics.update({self.get_id(): self})
