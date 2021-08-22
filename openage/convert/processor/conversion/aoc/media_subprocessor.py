# Copyright 2019-2021 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-few-public-methods
"""
Convert media information to metadata definitions and export
requests. Subroutine of the main AoC processor.
"""
from openage.convert.entity_object.export.metadata_export import TextureMetadataExport
from openage.convert.value_object.read.media_types import MediaType

from ....entity_object.export.formats.sprite_metadata import LayerMode
from ....entity_object.export.media_export_request import MediaExportRequest
from ....entity_object.export.metadata_export import SpriteMetadataExport


class AoCMediaSubprocessor:
    """
    Creates the exports requests for media files from AoC.
    """

    @classmethod
    def convert(cls, full_data_set):
        """
        Create all export requests for the dataset.
        """
        cls.create_graphics_requests(full_data_set)
        # cls.create_blend_requests(full_data_set)
        cls.create_sound_requests(full_data_set)

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

            # Animation metadata file definiton
            sprite_meta_filename = f"{sprite.get_filename()}.sprite"
            sprite_meta_export = SpriteMetadataExport(sprite.resolve_sprite_location(),
                                                      sprite_meta_filename)
            full_data_set.metadata_exports.append(sprite_meta_export)

            for graphic in ref_graphics:
                graphic_id = graphic.get_id()
                if graphic_id in handled_graphic_ids:
                    continue

                # Texture image file definiton
                targetdir = graphic_targetdirs[graphic_id]
                source_filename = f"{str(graphic['slp_id'].get_value())}.slp"
                target_filename = "%s_%s.png" % (sprite.get_filename(),
                                                 str(graphic["slp_id"].get_value()))

                export_request = MediaExportRequest(MediaType.GRAPHICS,
                                                    targetdir,
                                                    source_filename,
                                                    target_filename)
                full_data_set.graphics_exports.update({graphic_id: export_request})

                # Texture metadata file definiton
                # Same file stem as the image file and same targetdir
                texture_meta_filename = f"{target_filename[:-4]}.texture"
                texture_meta_export = TextureMetadataExport(targetdir,
                                                            texture_meta_filename)
                full_data_set.metadata_exports.append(texture_meta_export)

                # Add texture image filename to texture metadata
                texture_meta_export.add_imagefile(target_filename)

                # Add metadata from graphics to animation metadata
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
                sprite_meta_export.add_graphics_metadata(target_filename,
                                                         texture_meta_filename,
                                                         layer_mode,
                                                         layer_pos,
                                                         frame_rate,
                                                         replay_delay,
                                                         frame_count,
                                                         angle_count,
                                                         mirror_mode)

                # Notify metadata export about SLP metadata when the file is exported
                export_request.add_observer(texture_meta_export)
                export_request.add_observer(sprite_meta_export)

                handled_graphic_ids.add(graphic_id)

        combined_terrains = full_data_set.combined_terrains.values()
        for texture in combined_terrains:
            slp_id = texture.get_id()

            targetdir = texture.resolve_graphics_location()
            source_filename = f"{str(slp_id)}.slp"
            target_filename = f"{texture.get_filename()}.png"

            export_request = MediaExportRequest(MediaType.TERRAIN,
                                                targetdir,
                                                source_filename,
                                                target_filename)
            full_data_set.graphics_exports.update({slp_id: export_request})

    @staticmethod
    def create_blend_requests(full_data_set):
        """
        Create export requests for Blendomatic objects.

        TODO: Blendomatic contains multiple files. Better handling?
        """
        export_request = MediaExportRequest(
            MediaType.BLEND,
            "data/blend/",
            full_data_set.game_version[0].media_paths[MediaType.BLEND][0],
            "blendmode"
        )
        full_data_set.blend_exports.update({0: export_request})

    @staticmethod
    def create_sound_requests(full_data_set):
        """
        Create export requests for sounds referenced by CombinedSound objects.
        """
        combined_sounds = full_data_set.combined_sounds.values()

        for sound in combined_sounds:
            sound_id = sound.get_file_id()

            targetdir = sound.resolve_sound_location()
            source_filename = f"{str(sound_id)}.wav"
            target_filename = f"{sound.get_filename()}.opus"

            export_request = MediaExportRequest(MediaType.SOUNDS,
                                                targetdir,
                                                source_filename,
                                                target_filename)

            full_data_set.sound_exports.update({sound_id: export_request})
