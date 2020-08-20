# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-arguments,too-many-locals

"""
Export requests for media metadata.
"""
# REFA: Whole file -> entity object

from ...util.observer import Observer
from .formats.sprite_metadata import SpriteMetadata


class MetadataExport(Observer):
    """
    A class for exporting metadata from another format. MetadataExports are
    observers so they can receive data from media conversion.
    """

    def __init__(self, targetdir, target_filename):

        self.targetdir = targetdir
        self.target_filename = target_filename

    def save(self, exportdir, game_version=None):
        """
        Output the metadata into the associated file format(s).

        :param exportdir: Relative path to the export directory.
        :type exportdir: ...util.fslike.path.Path
        """
        raise NotImplementedError("%s has not implemented save()"
                                  % (self))

    def __repr__(self):
        return "MetadataExport<%s>" % (type(self))


class SpriteMetadataExport(MetadataExport):
    """
    Export requests for sprite definition files.
    """

    def __init__(self, targetdir, target_filename):
        super().__init__(targetdir, target_filename)

        self.graphics_metadata = {}
        self.frame_metadata = {}

    def add_graphics_metadata(self, img_filename, layer_mode,
                              layer_pos, frame_rate, replay_delay,
                              frame_count, angle_count, mirror_mode):
        """
        Add metadata from the GenieGraphic object.

        :param img_filename: Filename of the exported PNG file.
        """
        self.graphics_metadata[img_filename] = (layer_mode, layer_pos, frame_rate, replay_delay,
                                                frame_count, angle_count, mirror_mode)

    def save(self, exportdir, game_version=None):
        sprite_file = SpriteMetadata(self.targetdir, self.target_filename)

        index = 0
        for img_filename, metadata in self.graphics_metadata.items():
            sprite_file.add_image(index, img_filename)
            sprite_file.add_layer(index, *metadata[:4])

            degree = 0
            frame_count = metadata[4]
            angle_count = metadata[5]
            mirror_mode = metadata[6]
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

                        sprite_file.add_frame(index, int(degree), index, *frame_metadata.values())

                degree += degree_step

            index += 1

        sprite_file.save(exportdir)

    def update(self, observable, message=None):
        """
        Receive metdata from the graphics file export.

        :param message: A dict with frame metadata from the exported PNG file.
        :type message: dict
        """
        if message:
            for img_filename, frame_metadata in message.items():
                self.frame_metadata[img_filename] = frame_metadata
