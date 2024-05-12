# Copyright 2019-2024 the openage authors. See copying.md for legal info.

"""
Contains structures and API-like objects for terrain from AoC.
"""
from __future__ import annotations
import typing

from ..converter_object import ConverterObject, ConverterObjectGroup

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_object_container\
        import GenieObjectContainer
    from openage.convert.value_object.read.value_members import ValueMember


class GenieTerrainObject(ConverterObject):
    """
    Terrain definition from a .dat file.
    """

    __slots__ = ('data',)

    def __init__(
        self,
        terrain_id: int,
        full_data_set: GenieObjectContainer,
        members: dict[str, ValueMember] = None
    ):
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
        return f"GenieTerrainObject<{self.get_id()}>"


class GenieTerrainGroup(ConverterObjectGroup):
    """
    A terrain from AoE that will become an openage Terrain object.
    """

    __slots__ = ('data', 'terrain')

    def __init__(
        self,
        terrain_id: int,
        full_data_set: GenieObjectContainer,
    ):
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

    def has_subterrain(self) -> bool:
        """
        Checks if this terrain uses a subterrain for its graphics.
        """
        return self.terrain["terrain_replacement_id"].value > -1

    def get_subterrain(self) -> GenieTerrainObject:
        """
        Return the subterrain used for the graphics.
        """
        return self.data.genie_terrains[self.terrain["terrain_replacement_id"].value]

    def get_terrain(self) -> GenieTerrainObject:
        """
        Return the subterrain used for the graphics.
        """
        return self.terrain

    def __repr__(self):
        return f"GenieTerrainGroup<{self.get_id()}>"


class GenieTerrainRestriction(ConverterObject):
    """
    Terrain restriction definition from a .dat file.
    """

    __slots__ = ('data',)

    def __init__(
        self,
        restriction_id: int,
        full_data_set: GenieObjectContainer,
        members: dict[str, ValueMember] = None
    ):
        """
        Creates a new Genie terrain restriction object.

        :param restriction_id: The index of the terrain restriction in the .dat file.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """
        super().__init__(restriction_id, members=members)

        self.data = full_data_set

    def is_accessible(self, terrain_index: int) -> bool:
        """
        Checks if a terrain is accessible by this restriction.

        :param terrain_index: Index of the terrain.
        """
        multiplier = self.members["accessible_dmgmultiplier"][terrain_index].value

        return multiplier > 0

    def __repr__(self):
        return f"GenieTerrainRestriction<{self.get_id()}>"
