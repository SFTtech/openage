# Copyright 2019-2020 the openage authors. See copying.md for legal info.

"""
Convert media information to metadata definitions and export
requests. Subroutine of the main AoC processor.
"""
from openage.convert.export.media_export_request import GraphicsMediaExportRequest,\
    SoundMediaExportRequest, TerrainMediaExportRequest


class AoCMediaSubprocessor:

    @classmethod
    def convert(cls, full_data_set):

        cls._create_graphics_requests(full_data_set)
        cls._create_sound_requests(full_data_set)

    @staticmethod
    def _create_graphics_requests(full_data_set):
        """
        Create export requests for graphics referenced by CombinedSprite objects.
        """
        combined_sprites = full_data_set.combined_sprites.values()
        handled_graphic_ids = set()

        for sprite in combined_sprites:
            ref_graphics = sprite.get_graphics()
            graphic_targetdirs = sprite.resolve_graphics_location()

            for graphic in ref_graphics:
                graphic_id = graphic.get_id()
                if graphic_id in handled_graphic_ids:
                    continue

                targetdir = graphic_targetdirs[graphic_id]
                source_filename = "%s.slp" % str(graphic.get_member("slp_id").get_value())
                target_filename = "%s_%s.png" % (sprite.get_filename(),
                                                 str(graphic.get_member("slp_id").get_value()))

                export_request = GraphicsMediaExportRequest(targetdir, source_filename,
                                                            target_filename)
                full_data_set.graphics_exports.update({graphic_id: export_request})

                handled_graphic_ids.add(graphic_id)

        combined_terrains = full_data_set.combined_terrains.values()
        for texture in combined_terrains:
            slp_id = texture.get_id()

            targetdir = texture.resolve_graphics_location()
            source_filename = "%s.slp" % str(slp_id)
            target_filename = "%s.png" % texture.get_filename()

            export_request = TerrainMediaExportRequest(targetdir, source_filename,
                                                       target_filename)
            full_data_set.graphics_exports.update({slp_id: export_request})

    @staticmethod
    def _create_sound_requests(full_data_set):
        """
        Create export requests for sounds referenced by CombinedSound objects.
        """
        combined_sounds = full_data_set.combined_sounds.values()

        for sound in combined_sounds:
            sound_id = sound.get_file_id()

            targetdir = sound.resolve_sound_location()
            source_filename = "%s.wav" % str(sound_id)
            target_filename = "%s.opus" % sound.get_filename()

            export_request = SoundMediaExportRequest(targetdir, source_filename,
                                                     target_filename)

            full_data_set.sound_exports.update({sound_id: export_request})
