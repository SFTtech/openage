# Copyright 2020-2021 the openage authors. See copying.md for legal info.

"""
References a graphic in the game that has to be converted.
"""


class CombinedSprite:
    """
    Collection of sprite information for openage files.

    This will become a spritesheet texture with a sprite file.
    """

    __slots__ = ('head_sprite_id', 'filename', 'data', 'metadata', '_refs')

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

        self.metadata = None

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

    def add_metadata(self, metadata):
        """
        Add a metadata file to the sprite.
        """
        self.metadata = metadata

    def get_filename(self):
        """
        Returns the desired filename of the sprite.
        """
        return self.filename

    def get_graphics(self):
        """
        Return all graphics referenced by this sprite.
        """
        graphics = [self.data.genie_graphics[self.head_sprite_id]]
        graphics.extend(self.data.genie_graphics[self.head_sprite_id].get_subgraphics())

        # Only consider existing graphics
        existing_graphics = []
        for graphic in graphics:
            if graphic.exists:
                existing_graphics.append(graphic)

        return existing_graphics

    def get_id(self):
        """
        Returns the head sprite ID of the sprite.
        """
        return self.head_sprite_id

    def get_relative_sprite_location(self):
        """
        Return the sprite file location relative to where the file
        is expected to be in the modpack.
        """
        if len(self._refs) > 1:
            return f"../shared/graphics/{self.filename}.sprite"

        if len(self._refs) == 1:
            return f"./graphics/{self.filename}.sprite"

        return None

    def remove_reference(self, referer):
        """
        Remove an object that is referencing this sprite.
        """
        self._refs.remove(referer)

    def resolve_graphics_location(self):
        """
        Returns the planned location in the modpack of all image files
        referenced by the sprite.
        """
        location_dict = {}

        for graphic in self.get_graphics():
            if graphic.is_shared():
                location_dict.update({graphic.get_id(): "data/game_entity/shared/graphics/"})

            else:
                location_dict.update({graphic.get_id(): self.resolve_sprite_location()})

        return location_dict

    def resolve_sprite_location(self):
        """
        Returns the planned location of the definition file in the modpack.
        """
        if len(self._refs) > 1:
            return "data/game_entity/shared/graphics/"

        if len(self._refs) == 1:
            return f"{self._refs[0].get_file_location()[0]}{'graphics/'}"

        return None

    def __repr__(self):
        return f"CombinedSprite<{self.head_sprite_id}>"
