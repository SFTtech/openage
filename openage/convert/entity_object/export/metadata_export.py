# Copyright 2020-2022 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-arguments,too-many-locals

"""
Export requests for media metadata.
"""
from __future__ import annotations
import typing


from ....util.observer import Observer
from .formats.sprite_metadata import SpriteMetadata

if typing.TYPE_CHECKING:
    from openage.util.observer import Observable
    from openage.convert.entity_object.export.formats.sprite_metadata import LayerMode


class MetadataExport(Observer):
    """
    A class for exporting metadata from another format. MetadataExports are
    observers so they can receive data from media conversion.
    """

    def __init__(self, targetdir: str, target_filename: str):

        self.targetdir = targetdir
        self.filename = target_filename

    def update(self, observable: Observable, message=None):
        return NotImplementedError("Interface does not implement update()")

    def __repr__(self):
        return f"MetadataExport<{type(self)}>"


class SpriteMetadataExport(MetadataExport):
    """
    Export requests for sprite definition files.
    """

    def __init__(self, targetdir, target_filename):
        super().__init__(targetdir, target_filename)

        self.graphics_metadata: dict[int, tuple] = {}
        self.frame_metadata: dict[int, tuple] = {}

    def add_graphics_metadata(
        self,
        img_filename: str,
        layer_mode: LayerMode,
        layer_pos: int,
        frame_rate: float,
        replay_delay: float,
        frame_count: int,
        angle_count: int,
        mirror_mode: int
    ):
        """
        Add metadata from the GenieGraphic object.

        :param img_filename: Filename of the exported PNG file.
        """
        self.graphics_metadata[img_filename] = (layer_mode, layer_pos, frame_rate, replay_delay,
                                                frame_count, angle_count, mirror_mode)

    def dump(self) -> str:
        """
        Creates a human-readable string that can be written to a file.
        """
        sprite_file = SpriteMetadata(self.targetdir, self.filename)

        img_index = 0
        for img_filename, metadata in self.graphics_metadata.items():
            sprite_file.add_image(img_index, img_filename)
            sprite_file.add_layer(img_index, *metadata[:4])

            degree = 0
            frame_count = metadata[4]
            angle_count = metadata[5]
            mirror_mode = metadata[6]

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
                        frame_metadata_index = frame_idx + angle_index * frame_count
                        if frame_metadata_index >= len(self.frame_metadata[img_filename]):
                            # TODO: Can happen for some death and projectile animations. Why?
                            break

                        frame_metadata = self.frame_metadata[img_filename][frame_metadata_index]

                        sprite_file.add_frame(
                            frame_idx,
                            int(degree),
                            img_index,
                            img_index,
                            *frame_metadata.values()
                        )

                degree += degree_step

            img_index += 1

        return sprite_file.dump()

    def update(self, observable, message=None):
        """
        Receive metdata from the graphics file export.

        :param message: A dict with frame metadata from the exported PNG file.
        :type message: dict
        """
        if message:
            for img_filename, frame_metadata in message.items():
                self.frame_metadata[img_filename] = frame_metadata


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
        sprite_file = SpriteMetadata(self.targetdir, self.filename)

        index = 0
        for img_filename, metadata in self.graphics_metadata.items():
            sprite_file.add_image(index, img_filename)
            sprite_file.add_layer(index, *metadata[:4])

            degree = 0
            frame_count = metadata[4]
            angle_count = metadata[5]
            mirror_mode = metadata[6]

            if angle_count == 0:
                angle_count = 1

            degree_step = 360 / angle_count
            for angle in range(angle_count):
                mirror_from = None
                if mirror_mode:
                    if degree > 180:
                        mirrored_angle = (angle - angle_count) * (-1)
                        mirror_from = int(mirrored_angle * degree_step)

                sprite_file.add_angle(int(degree), mirror_from)

                if not mirror_from:
                    for frame_idx in range(frame_count):
                        if frame_idx == len(self.frame_metadata[img_filename]):
                            # TODO: Can happen for some death animation. Why?
                            break

                        frame_metadata = self.frame_metadata[img_filename][frame_idx]

                        sprite_file.add_frame(
                            frame_idx,
                            int(degree),
                            index,
                            index,
                            *frame_metadata.values()
                        )

                degree += degree_step

            index += 1

        return sprite_file.dump()

    def update(self, observable: Observable, message: dict = None):
        """
        Receive metdata from the graphics file export.

        :param message: A dict with texture metadata from the exported PNG file.
        :type message: dict
        """
        if message:
            self.size = message["size"]
            self.subtex_metadata = message["subtex_metadata"]
