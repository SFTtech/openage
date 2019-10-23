# Copyright 2019-2019 the openage authors. See copying.md for legal info.


from ...dataformat.converter_object import ConverterObject


class GenieTerrainObject(ConverterObject):
    """
    Terrain definition from a .dat file.
    """

    def __init__(self, terrain_id, full_data_set, members=None):
        """
        Creates a new Genie terrain object.

        :param terrain_id: The index of the terrain in the .dat file's terrain
                           block. (the index is referenced by other terrains)
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :param members: An already existing member dict.
        """

        super().__init__(terrain_id, members=members)

        self.data = full_data_set
        self.data.genie_terrains.update({self.get_id(): self})
