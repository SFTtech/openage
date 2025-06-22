# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
References a sound in the game that has to be converted.
"""

from __future__ import annotations

import typing


if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.converter_object import ConverterObjectContainer
    from openage.convert.entity_object.conversion.converter_object import ConverterObject


class CombinedSound:
    """
    Collection of sound information for openage files.
    """

    __slots__ = ('head_sound_id', 'file_id', 'filename', 'data', 'genie_sound', '_refs')

    def __init__(
        self,
        head_sound_id: int,
        file_id: int,
        filename: str,
        full_data_set: ConverterObjectContainer
    ):
        """
        Creates a new CombinedSound instance.

        :param head_sound_id: The id of the GenieSound object of this sound.
        :type head_sound_id: int
        :param file_id: The id of the file resource in the GenieSound.
        :type file_id: int
        :param filename: Name of the sound file.
        :type filename: str
        :param full_data_set: ConverterObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.converter_object.ConverterObjectContainer
        """

        self.head_sound_id = head_sound_id
        self.file_id = file_id
        self.filename = filename
        self.data = full_data_set

        self.genie_sound = self.data.genie_sounds[self.head_sound_id]

        # Depending on the amounts of references:
        # 0 = do not convert;
        # 1 = store with GameEntity;
        # >1 = store in 'shared' resources;
        self._refs = []

    def add_reference(self, referer: ConverterObject) -> None:
        """
        Add an object that is referencing this sound.
        """
        self._refs.append(referer)

    def get_filename(self) -> str:
        """
        Returns the desired filename of the sprite.
        """
        return self.filename

    def get_file_id(self) -> int:
        """
        Returns the ID of the sound file in the game folder.
        """
        return self.file_id

    def get_id(self) -> int:
        """
        Returns the ID of the sound object in the .dat.
        """
        return self.head_sound_id

    def get_relative_file_location(self) -> str:
        """
        Return the sound file location relative to where the file
        is expected to be in the modpack.
        """
        if len(self._refs) > 1:
            return f"../../shared/sounds/{self.filename}.opus"

        if len(self._refs) == 1:
            return f"./sounds/{self.filename}.opus"

        return None

    def resolve_sound_location(self) -> str:
        """
        Returns the planned location of the sound file in the modpack.
        """
        if len(self._refs) > 1:
            return "data/game_entity/shared/sounds/"

        if len(self._refs) == 1:
            return f"{self._refs[0].get_file_location()[0]}{'sounds/'}"

        return None

    def remove_reference(self, referer: ConverterObject) -> None:
        """
        Remove an object that is referencing this sound.
        """
        self._refs.remove(referer)

    def __repr__(self):
        return f"CombinedSound<{self.head_sound_id}>"
