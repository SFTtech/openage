# Copyright 2019-2020 the openage authors. See copying.md for legal info.

"""
Convert media information to metadata definitions and export
requests. Subroutine of the main AoC processor.
"""
from openage.convert.export.media_export_request import MediaExportRequest
from openage.convert.dataformat.media_types import MediaType
from openage.convert.export.formats.sprite_metadata import SpriteMetadata,\
    LayerMode


class AoCMediaSubprocessor:

    @classmethod
    def convert(cls, full_data_set):

        cls._create_sprites(full_data_set)

    @classmethod
    def _create_sprites(cls, full_data_set):
        """
        Create sprite metadata and PNG files from CombinedSprite objects.
        """
        combined_sprites = full_data_set.combined_sprites.values()

        for sprite in combined_sprites:
            cls._create_sprite_media_request(sprite, full_data_set)
            cls._create_sprite_metadata(sprite, full_data_set)

    @staticmethod
    def _create_sprite_media_request(combined_sprite, full_data_set):
        """
        Create a request for the exporter to convert and output a graphics file.
        """
        graphic_id = combined_sprite.get_id()
        graphic = full_data_set.genie_graphics[graphic_id]

        # Export request for head graphic
        targetdir = combined_sprite.resolve_location()
        source_filename = graphic.get_member("filename").get_value()
        target_filename = combined_sprite.get_filename()

        export_request = MediaExportRequest(MediaType.GRAPHICS, targetdir,
                                            source_filename, target_filename)
        full_data_set.graphics_exports.update({graphic_id: export_request})

        # TODO: Deltas

    @staticmethod
    def _create_sprite_metadata(combined_sprite, full_data_set):
        """
        Metadata extraction. Not everything can be figured out here.
        Some of the info has to be fetched from the SLP/SMX files.
        """
        graphic_id = combined_sprite.get_id()
        graphic = full_data_set.genie_graphics[graphic_id]

        targetdir = combined_sprite.resolve_location()
        target_filename = combined_sprite.get_filename()
        img_index = 0

        # Image
        sprite_metadata = SpriteMetadata(targetdir, target_filename)
        sprite_metadata.add_image(img_index, target_filename)

        # Layer
        mode = graphic.get_member("sequence_type").get_value()

        layer_mode = LayerMode.OFF
        if mode & 0x01:
            layer_mode = LayerMode.LOOP
        if mode & 0x08:
            layer_mode = LayerMode.ONCE

        layer_pos = graphic.get_member("layer").get_value()
        time_per_frame = graphic.get_member("frame_rate").get_value()
        replay_delay = graphic.get_member("replay_delay").get_value()

        sprite_metadata.add_layer(img_index, layer_mode, layer_pos,
                                  time_per_frame, replay_delay)

        # Angles
        if mode & 0x02 and not mode & 0x04:
            angle_count = graphic.get_member("angle_count").get_value()
            angle_step = 360 / angle_count
            angle = 0

            while angle <= 180:
                sprite_metadata.add_angle(angle)
                angle += angle_step

            while angle < 360:
                sprite_metadata.add_angle(angle, mirror_from=(angle - 180))
                angle += angle_step

        else:
            sprite_metadata.add_angle(45)

        combined_sprite.add_metadata(sprite_metadata)

        # TODO: Deltas
