# Copyright 2019-2020 the openage authors. See copying.md for legal info.

"""
Contains structures and API-like objects for terrain from AoC.
"""


from ..converter_object import ConverterObject, ConverterObjectGroup


class GenieTerrainObject(ConverterObject):
    """
    Terrain definition from a .dat file.
    """

    __slots__ = ('data',)

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

    def __repr__(self):
        return "GenieTerrainObject<%s>" % (self.get_id())


class GenieTerrainGroup(ConverterObjectGroup):
    """
    A terrain from AoE that will become an openage Terrain object.
    """

    __slots__ = ('data', 'terrain')

    def __init__(self, terrain_id, full_data_set):
        """
        Creates a new Genie tech group object.

        :param terrain_id: The index of the terrain in the .dat file's terrain table.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(terrain_id)

        self.data = full_data_set

        # The terrain that belongs to the index
        self.terrain = self.data.genie_terrains[terrain_id]

    def has_subterrain(self):
        """
        Checks if this terrain uses a subterrain for its graphics.
        """
        return self.terrain["terrain_replacement_id"].get_value() > -1

    def get_subterrain(self):
        """
        Return the subterrain used for the graphics.
        """
        return self.data.genie_terrains[self.terrain["terrain_replacement_id"].get_value()]

    def get_terrain(self):
        """
        Return the subterrain used for the graphics.
        """
        return self.terrain

    def __repr__(self):
        return "GenieTerrainGroup<%s>" % (self.get_id())
