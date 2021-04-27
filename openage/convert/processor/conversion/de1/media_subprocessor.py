# Copyright 2020-2021 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals
"""
Convert media information to metadata definitions and export
requests. Subroutine of the main DE1 processor.
"""
from ....entity_object.export.formats.sprite_metadata import LayerMode
from ....entity_object.export.media_export_request import MediaExportRequest
from ....entity_object.export.metadata_export import SpriteMetadataExport
from ....value_object.read.media_types import MediaType
from ..aoc.media_subprocessor import AoCMediaSubprocessor


class DE1MediaSubprocessor:
    """
    Creates the exports requests for media files from DE1.
    """

    @classmethod
    def convert(cls, full_data_set):
        """
        Create all export requests for the dataset.
        """
        cls.create_graphics_requests(full_data_set)
        AoCMediaSubprocessor.create_sound_requests(full_data_set)

    @staticmethod
    def create_graphics_requests(full_data_set):
        """
        Create export requests for graphics referenced by CombinedSprite objects.
        """
        combined_sprites = full_data_set.combined_sprites.values()
        handled_graphic_ids = set()

        for sprite in combined_sprites:
            ref_graphics = sprite.get_graphics()
            graphic_targetdirs = sprite.resolve_graphics_location()

            metadata_filename = f"{sprite.get_filename()}.{'sprite'}"
            metadata_export = SpriteMetadataExport(sprite.resolve_sprite_location(),
                                                   metadata_filename)
            full_data_set.metadata_exports.append(metadata_export)

            for graphic in ref_graphics:
                graphic_id = graphic.get_id()
                if graphic_id in handled_graphic_ids:
                    continue

                targetdir = graphic_targetdirs[graphic_id]

                # DE1 stores most graphics filenames as 'whatever_<x#>'
                # where '<x#>' must be replaced by x1, x2 or x4
                # which corresponds to the graphics resolution variant
                source_str = graphic['filename'].get_value()[:-4]

                # TODO: Also convert x2 and x4 variants
                source_filename = f"{source_str}x1.slp"
                target_filename = "%s_%s.png" % (sprite.get_filename(),
                                                 str(graphic["slp_id"].get_value()))

                export_request = MediaExportRequest(MediaType.GRAPHICS,
                                                    targetdir,
                                                    source_filename,
                                                    target_filename)
                full_data_set.graphics_exports.update({graphic_id: export_request})

                # Metadata from graphics
                sequence_type = graphic["sequence_type"].get_value()
                if sequence_type == 0x00:
                    layer_mode = LayerMode.OFF

                elif sequence_type & 0x08:
                    layer_mode = LayerMode.ONCE

                else:
                    layer_mode = LayerMode.LOOP

                layer_pos = graphic["layer"].get_value()
                frame_rate = round(graphic["frame_rate"].get_value(), ndigits=6)
                if frame_rate < 0.000001:
                    frame_rate = None

                replay_delay = round(graphic["replay_delay"].get_value(), ndigits=6)
                if replay_delay < 0.000001:
                    replay_delay = None

                frame_count = graphic["frame_count"].get_value()
                angle_count = graphic["angle_count"].get_value()
                mirror_mode = graphic["mirroring_mode"].get_value()
                metadata_export.add_graphics_metadata(target_filename,
                                                      layer_mode,
                                                      layer_pos,
                                                      frame_rate,
                                                      replay_delay,
                                                      frame_count,
                                                      angle_count,
                                                      mirror_mode)

                # Notify metadata export about SMX metadata when the file is exported
                export_request.add_observer(metadata_export)

                handled_graphic_ids.add(graphic_id)

        # TODO: Terrains
