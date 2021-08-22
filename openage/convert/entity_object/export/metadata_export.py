# Copyright 2020-2021 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-arguments,too-many-locals
"""
Export requests for media metadata.
"""

from ....util.observer import Observer
from .formats.sprite_metadata import SpriteMetadata
from .formats.texture_metadata import TextureMetadata


class MetadataExport(Observer):
    """
    A class for exporting metadata from another format. MetadataExports are
    observers so they can receive data from media conversion.
    """

    def __init__(self, targetdir, target_filename):

        self.targetdir = targetdir
        self.filename = target_filename

    def update(self, observable, message=None):
        return NotImplemented

    def __repr__(self):
        return f"MetadataExport<{type(self)}>"


class SpriteMetadataExport(MetadataExport):
    """
    Export requests for sprite definition files.
    """

    def __init__(self, targetdir, target_filename):
        super().__init__(targetdir, target_filename)

        self.graphics_metadata = {}
        self.subtex_count = {}

    def add_graphics_metadata(self, img_filename, tex_filename, layer_mode,
                              layer_pos, frame_rate, replay_delay,
                              frame_count, angle_count, mirror_mode):
        """
        Add metadata from the GenieGraphic object.

        :param tex_filename: Filename of the .texture file.
        """
        self.graphics_metadata[img_filename] = (tex_filename, layer_mode, layer_pos, frame_rate,
                                                replay_delay, frame_count, angle_count, mirror_mode)

    def dump(self):
        """
        Creates a human-readable string that can be written to a file.
        """
        sprite_file = SpriteMetadata(self.targetdir, self.filename)

        tex_index = 0
        for img_filename, metadata in self.graphics_metadata.items():
            tex_filename = metadata[0]
            sprite_file.add_texture(tex_index, tex_filename)
            sprite_file.add_layer(tex_index, *metadata[1:5])

            degree = 0
            frame_count = metadata[5]
            angle_count = metadata[6]
            mirror_mode = metadata[7]

            if angle_count == 0:
                angle_count = 1

            degree_step = 360 / angle_count
            for angle_index in range(angle_count):
                mirror_from = None
                if mirror_mode:
                    if degree > 180:
                        mirrored_angle = (angle_index - angle_count) * (-1)
                        mirror_from = int(mirrored_angle * degree_step)

                sprite_file.add_angle(int(degree), mirror_from)

                if not mirror_from:
                    for frame_idx in range(frame_count):
                        subtex_index = frame_idx + angle_index * frame_count
                        if subtex_index >= self.subtex_count[img_filename]:
                            # TODO: Can happen for some death and projectile animations. Why?
                            break

                        sprite_file.add_frame(
                            frame_idx,
                            int(degree),
                            tex_index,
                            tex_index,
                            subtex_index
                        )

                degree += degree_step

            tex_index += 1

        return sprite_file.dump()

    def update(self, observable, message=None):
        """
        Receive metdata from the graphics file export.

        :param message: A dict with frame metadata from the exported PNG file.
        :type message: dict
        """
        if message:
            for tex_filename, metadata in message.items():
                self.subtex_count[tex_filename] = len(metadata["subtex_metadata"])


class TextureMetadataExport(MetadataExport):
    """
    Export requests for texture definition files.
    """

    def __init__(self, targetdir, target_filename):
        super().__init__(targetdir, target_filename)

        self.imagefile = None
        self.size = None
        self.pxformat = "rgba8"
        self.cbits = True
        self.subtex_metadata = []

    def add_imagefile(self, img_filename):
        """
        Add metadata from the GenieGraphic object.

        :param img_filename: Filename of the exported PNG file.
        """
        self.imagefile = img_filename

    def dump(self):
        """
        Creates a human-readable string that can be written to a file.
        """
        texture_file = TextureMetadata(self.targetdir, self.filename)

        texture_file.set_imagefile(self.imagefile)
        texture_file.set_size(self.size[0], self.size[1])
        texture_file.set_pxformat(self.pxformat, self.cbits)

        for subtex_metadata in self.subtex_metadata:
            texture_file.add_subtex(*subtex_metadata.values())

        return texture_file.dump()

    def update(self, observable, message=None):
        """
        Receive metdata from the graphics file export.

        :param message: A dict with texture metadata from the exported PNG file.
        :type message: dict
        """
        if message:
            texture_metadata = message[self.imagefile]
            self.size = texture_metadata["size"]
            self.subtex_metadata = texture_metadata["subtex_metadata"]
