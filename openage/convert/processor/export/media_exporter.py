# Copyright 2021-2021 the openage authors. See copying.md for legal info.

"""
Converts media requested by export requests to files.
"""
from openage.convert.entity_object.export.texture import Texture
from openage.convert.value_object.read.media_types import MediaType


class MediaExporter:
    """
    Provides functions for convertingmedia files and writing them to a targetdir.
    """

    @staticmethod
    def export(export_requests, sourcedir, exportdir, args):
        """
        Converts files requested by a MediaExportRequests.
        """
        for media_type in export_requests.keys():
            cur_export_requests = export_requests[media_type]

            export_func = None
            kwargs = {}
            if media_type is MediaType.TERRAIN:
                # Game version and palettes
                kwargs["game_version"] = args.game_version
                kwargs["palettes"] = args.palettes
                kwargs["compression_level"] = args.compression_level
                export_func = MediaExporter._export_terrain

            elif media_type is MediaType.GRAPHICS:
                kwargs["palettes"] = args.palettes
                kwargs["compression_level"] = args.compression_level
                export_func = MediaExporter._export_graphics

            elif media_type is MediaType.SOUNDS:
                export_func = MediaExporter._export_sound

            for request in cur_export_requests:
                export_func(request, sourcedir, exportdir, **kwargs)

    @staticmethod
    def _export_blend(export_request, sourcedir, exportdir):
        """
        Convert and export a blend file.
        """

    @staticmethod
    def _export_graphics(export_request, sourcedir, exportdir, palettes, compression_level):
        """
        Convert and export a graphics file.
        """
        source_file = sourcedir[
            export_request.get_type().value,
            export_request.source_filename
        ]

        try:
            media_file = source_file.open("rb")

        except FileNotFoundError:
            if source_file.suffix.lower() == ".smx":
                # Rename extension to SMP and try again
                other_filename = export_request.source_filename[:-1] + "p"
                source_file = sourcedir[
                    export_request.get_type().value,
                    other_filename
                ]

            media_file = source_file.open("rb")

        if source_file.suffix.lower() == ".slp":
            from ...value_object.read.media.slp import SLP
            image = SLP(media_file.read())

        elif source_file.suffix.lower() == ".smp":
            from ...value_object.read.media.smp import SMP
            image = SMP(media_file.read())

        elif source_file.suffix.lower() == ".smx":
            from ...value_object.read.media.smx import SMX
            image = SMX(media_file.read())

        texture = Texture(image, palettes)
        metadata = texture.save(
            exportdir.joinpath(export_request.targetdir),
            export_request.target_filename,
            compression_level
        )
        metadata = {export_request.target_filename: metadata}

        export_request.set_changed()
        export_request.notify_observers(metadata)
        export_request.clear_changed()

    @staticmethod
    def _export_interface(export_request, sourcedir, **kwargs):
        """
        Convert and export a sound file.
        """

    @staticmethod
    def _export_sound(export_request, sourcedir, exportdir):
        """
        Convert and export a sound file.
        """
        source_file = sourcedir[
            export_request.get_type().value,
            export_request.source_filename
        ]

        if source_file.is_file():
            with source_file.open_r() as infile:
                media_file = infile.read()

        else:
            # TODO: Filter files that do not exist out sooner
            return

        from ...service.export.opus.opusenc import encode

        soundata = encode(media_file)

        if isinstance(soundata, (str, int)):
            raise Exception(f"opusenc failed: {soundata}")

        export_file = exportdir[
            export_request.targetdir,
            export_request.target_filename
        ]

        with export_file.open_w() as outfile:
            outfile.write(soundata)

    @staticmethod
    def _export_terrain(export_request, sourcedir, exportdir, palettes,
                        game_version, compression_level):
        """
        Convert and export a terrain graphics file.
        """
        source_file = sourcedir[
            export_request.get_type().value,
            export_request.source_filename
        ]
        media_file = source_file.open("rb")

        if source_file.suffix.lower() == ".slp":
            from ...value_object.read.media.slp import SLP
            image = SLP(media_file.read())

        elif source_file.suffix.lower() == ".dds":
            # TODO: Implement
            pass

        if game_version[0].game_id in ("AOC", "SWGB"):
            from ...processor.export.texture_merge import merge_terrain
            texture = Texture(image, palettes, custom_merger=merge_terrain)

        else:
            texture = Texture(image, palettes)

        texture.save(
            exportdir.joinpath(export_request.targetdir),
            export_request.target_filename,
            compression_level
        )
