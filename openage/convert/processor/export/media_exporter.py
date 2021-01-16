# Copyright 2021-2021 the openage authors. See copying.md for legal info.

"""
Converts media requested by export requests to files.
"""
import os

from openage.convert.entity_object.export.texture import Texture
from openage.convert.service import debug_info
from openage.convert.service.export.load_replay_data import load_graphics_replay_data
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
        replay_info = {}
        if args.game_version[0].replay_graphics:
            replay_info = load_graphics_replay_data(args.game_version[0].replay_graphics)

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
                kwargs["replay_info"] = replay_info
                export_func = MediaExporter._export_graphics

            elif media_type is MediaType.SOUNDS:
                export_func = MediaExporter._export_sound

            for request in cur_export_requests:
                export_func(request, sourcedir, exportdir, **kwargs)

        if args.debug_info == 3:
            replaydata = {}
            for request in export_requests[MediaType.GRAPHICS]:
                kwargs = {}
                kwargs["palettes"] = args.palettes
                kwargs["compression_level"] = args.compression_level

                replay = MediaExporter._get_graphics_replay_data(
                    request,
                    sourcedir,
                    args.palettes,
                    compression_level=2
                )

                replaydata[request] = replay

            debug_info.debug_graphics_replay(
                args.debugdir,
                args.debug_info,
                sourcedir,
                replaydata,
                args.game_version
            )

    @staticmethod
    def _export_blend(export_request, sourcedir, exportdir):
        """
        Convert and export a blend file.
        """
        # ASDF

    @staticmethod
    def _export_graphics(export_request, sourcedir, exportdir, palettes,
                         compression_level, replay_info=None):
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
        MediaExporter.save_png(
            texture,
            exportdir[export_request.targetdir],
            export_request.target_filename,
            compression_level,
        )
        metadata = {export_request.target_filename: texture.get_metadata()}

        export_request.set_changed()
        export_request.notify_observers(metadata)
        export_request.clear_changed()

    @staticmethod
    def _export_interface(export_request, sourcedir, **kwargs):
        """
        Convert and export a sound file.
        """
        # ASDF

    @staticmethod
    def _export_palette(export_request, sourcedir, **kwargs):
        """
        Convert and export a palette file.
        """
        # ASDF

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

        MediaExporter.save_png(
            texture,
            exportdir[export_request.targetdir],
            export_request.target_filename,
            compression_level,
        )

    @staticmethod
    def _get_graphics_replay_data(export_request, sourcedir, palettes, compression_level):
        """
        Convert a graphics file and return the used settings. This performs
        a dry run, i.e. the graphics media is not saved on the filesystem.
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
        MediaExporter.save_png(
            texture,
            None,
            None,
            compression_level=compression_level,
            replay=None,
            dry_run=True
        )

        return texture.get_replay_params()

    @staticmethod
    def save_png(texture, targetdir, filename, compression_level=1, replay=None, dry_run=False):
        """
        Store the image data into the target directory path,
        with given filename="dir/out.png".

        :param compression_level: Compression level of the PNG. A higher
                                  level results in smaller file sizes, but
                                  takes longer to generate.
                                      - 0 = no compression
                                      - 1 = normal png compression (default)
                                      - 2 = greedy search for smallest file; slowdown is 8x
                                      - 3 = maximum possible compression; slowdown is 256x
        :type compression_level: int
        :param dry_run: Create the PNG but don't save it as a file.
        :type dry_run: bool
        """
        from ...service.export.png import png_create

        COMPRESSION_LEVELS = {
            0: png_create.CompressionMethod.COMPR_NONE,
            1: png_create.CompressionMethod.COMPR_DEFAULT,
            2: png_create.CompressionMethod.COMPR_GREEDY,
            3: png_create.CompressionMethod.COMPR_AGGRESSIVE,
        }

        if not dry_run:
            _, ext = os.path.splitext(filename)

            # only allow png
            if ext != ".png":
                raise ValueError("Filename invalid, a texture must be saved"
                                 "as 'filename.png', not '%s'" % (filename))

        compression_method = COMPRESSION_LEVELS.get(
            compression_level,
            png_create.CompressionMethod.COMPR_DEFAULT
        )
        png_data, compr_params = png_create.save(
            texture.image_data.data,
            compression_method,
            replay
        )

        if not dry_run:
            with targetdir[filename].open("wb") as imagefile:
                imagefile.write(png_data)

        if compr_params:
            texture.best_compr = (compression_level, *compr_params)
