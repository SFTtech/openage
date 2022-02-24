# Copyright 2020-2022 the openage authors. See copying.md for legal info.

"""
References a graphic in the game that has to be converted.
"""

from __future__ import annotations

import typing


if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.converter_object import ConverterObjectContainer
    from openage.convert.entity_object.conversion.converter_object import ConverterObject
    from openage.convert.entity_object.conversion.aoc.genie_terrain import GenieTerrainObject


class CombinedTerrain:
    """
    Collection of terrain information for openage files.

    This will become a spritesheet texture with a terrain file.
    """

    __slots__ = ('terrain_id', 'filename', 'data', 'metadata', '_refs')

    def __init__(
        self,
        terrain_id: int,
        filename: str,
        full_data_set: ConverterObjectContainer
    ):
        """
        Creates a new CombinedTerrain instance.

        :param terrain_id: The index of the terrain that references the sprite.
        :type terrain_id: int
        :param filename: Name of the terrain and definition file.
        :type filename: str
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.converter_object.ConverterObjectContainer
        """

        self.terrain_id = terrain_id
        self.filename = filename
        self.data = full_data_set

        self.metadata = None

        # Depending on the amounts of references:
        # 0 = do not convert;
        # >=1 = store with first occuring Terrain;
        self._refs = []

    def add_reference(self, referer: ConverterObject) -> None:
        """
        Add an object that is referencing this terrain.
        """
        self._refs.append(referer)

    def get_filename(self) -> str:
        """
        Returns the destination filename of the terrain.
        """
        return self.filename

    def get_terrain(self) -> GenieTerrainObject:
        """
        Returns the terrain referenced by this terrain sprite.
        """
        return self.data.genie_terrains[self.terrain_id]

    def get_id(self) -> int:
        """
        Returns the terrain id of the terrain.
        """
        return self.terrain_id

    def get_relative_terrain_location(self) -> str:
        """
        Return the terrain file location relative to where the file
        is expected to be in the modpack.
        """
        if len(self._refs) >= 1:
            return f"./graphics/{self.filename}.terrain"

        return None

    def remove_reference(self, referer: ConverterObject) -> None:
        """
        Remove an object that is referencing this sprite.
        """
        self._refs.remove(referer)

    def resolve_graphics_location(self) -> str:
        """
        Returns the planned location in the modpack of the image file
        referenced by the terrain file.
        """
        return self.resolve_terrain_location()

    def resolve_terrain_location(self) -> str:
        """
        Returns the planned location of the definition file in the modpack.
        """
        if len(self._refs) >= 1:
            return f"{self._refs[0].get_file_location()[0]}{'graphics/'}"

        return None

    def __repr__(self):
        return f"CombinedTerrain<{self.terrain_id}>"
