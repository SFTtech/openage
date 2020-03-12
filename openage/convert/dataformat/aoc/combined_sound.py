# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
References a sound in the game that has to be converted.
"""


class CombinedSound:
    """
    Collection of sound information for openage files.
    """

    def __init__(self, head_sound_id, file_id, filename, full_data_set):
        """
        Creates a new CombinedSound instance.

        :param head_sound_id: The id of the GenieSound object of this sound.
        :type head_sound_id: int
        :param file_id: The id of the file resource in the GenieSound.
        :type file_id: int
        :param filename: Name of the sound file.
        :type filename: str
        :param full_data_set: GenieObjectContainer instance that
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

    def add_reference(self, referer):
        """
        Add an object that is referencing this sound.
        """
        self._refs.append(referer)

    def get_filename(self):
        """
        Returns the desired filename of the sprite.
        """
        return self.filename

    def get_file_id(self):
        """
        Returns the ID of the sound file in the game folder.
        """
        return self.file_id

    def get_id(self):
        """
        Returns the ID of the sound object in the .dat.
        """
        return self.head_sound_id

    def get_relative_file_location(self):
        """
        Return the sound file location relative to where the file
        is expected to be in the modpack.
        """
        if len(self._refs) > 1:
            return "../shared/sounds/%s.opus" % (self.filename)

        elif len(self._refs) == 1:
            return "./sounds/%s.opus" % (self.filename)

    def resolve_sound_location(self):
        """
        Returns the planned location of the sound file in the modpack.
        """
        if len(self._refs) > 1:
            return "data/game_entity/shared/sounds/"

        elif len(self._refs) == 1:
            return "%s%s" % (self._refs[0].get_file_location()[0], "sounds/")

        return None

    def remove_reference(self, referer):
        """
        Remove an object that is referencing this sound.
        """
        self._refs.remove(referer)

    def __repr__(self):
        return "CombinedSound<%s>" % (self.head_sound_id)
