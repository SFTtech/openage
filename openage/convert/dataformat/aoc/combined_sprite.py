# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
References a graphic in the game that has to be converted.
"""
from .expected_pointer import ExpectedPointer


class CombinedSprite:
    """
    Collection of sprite information for openage files.

    This will become a spritesheet texture with a sprite file.
    """

    def __init__(self, head_sprite_id, filename, full_data_set):
        """
        Creates a new CombinedSprite instance.

        :param head_sprite_id: The id of the top level graphic of this sprite.
        :type head_sprite_id: int
        :param filename: Name of the sprite and definition file.
        :type filename: str
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.converter_object.ConverterObjectContainer
        """

        self.head_sprite_id = head_sprite_id
        self.filename = filename
        self.data = full_data_set

        # Depending on the amounts of references:
        # 0 = do not convert;
        # 1 = store with GameEntity;
        # >1 = store in 'shared' resources;
        self._refs = []

    def add_reference(self, referer):
        """
        Add an object that is referencing this sprite.
        """
        self._refs.append(referer)

    def get_id(self):
        """
        Returns the head sprite ID of the sprite.
        """
        return self.head_sprite_id

    def remove_reference(self, referer):
        """
        Remove an object that is referencing this sprite.
        """
        self._refs.remove(referer)

    def resolve_location(self):
        """
        Returns the location of the definition file in the modpack
        """
        if len(self._refs) > 1:
            return "data/game_entity/shared/graphics/%s.sprite" % (self.filename)

        elif len(self._refs) == 1:
            return "%s/%s.sprite" % ("graphics", self.filename)

        return None

    def save(self):
        """
        Create a .sprite or .terrain definition and corresponding texture.
        """
        # TODO: Create SpriteFile(..) and TerrainFile() instances here.
        pass

    def __repr__(self):
        return "CombinedSprite<%s>" % (self.head_sprite_id)


def frame_to_seconds(frame_num, frame_rate):
    """
    Translates a number of frames to the time it takes to display
    them in the Genie Engine games. The framerate is defined by the
    individual graphics.

    :param frame_num: Number of frames.
    :type frame_num: int
    :param frame_rate: Time necesary to display a single frame.
    :type frame_rate: float
    """
    if frame_num < 0:
        raise Exception("Number of frames cannot be negative, received %s"
                        % (frame_num))

    if frame_rate < 0:
        raise Exception("Framerate cannot be negative, received %s"
                        % (frame_rate))

    return frame_num * frame_rate
