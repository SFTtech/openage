# Copyright 2021-2023 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-arguments,too-many-locals
"""
Converts media requested by export requests to files.
"""
from __future__ import annotations
import typing

import logging
import os


from openage.convert.entity_object.export.texture import Texture
from openage.convert.service import debug_info
from openage.convert.service.export.load_media_cache import load_media_cache
from openage.convert.value_object.read.media.blendomatic import Blendomatic
from openage.convert.value_object.read.media_types import MediaType
from openage.log import dbg, info, get_loglevel
from openage.util.strings import format_progress

if typing.TYPE_CHECKING:
    from argparse import Namespace

    from openage.convert.entity_object.export.media_export_request import MediaExportRequest
    from openage.convert.value_object.read.media.colortable import ColorTable
    from openage.convert.value_object.init.game_version import GameVersion
    from openage.util.fslike.path import Path


class MediaExporter:
    """
    Provides functions for converting media files and writing them to a targetdir.
    """

    @staticmethod
    def export(
        export_requests: dict[MediaType, list[MediaExportRequest]],
        sourcedir: Path,
        exportdir: Path,
        args: Namespace
    ) -> None:
        """
        Converts files requested by MediaExportRequests.

        :param export_requests: Export requests for media files. This is a dict of export requests
                                by their media type.
        :param sourcedir: Directory where all media assets are mounted. Source subfolder and
                          source filename should be stored in the export request.
        :param exportdir: Directory the resulting file(s) will be exported to. Target subfolder
                          and target filename should be stored in the export request.
        :param args: Converter arguments.
        :type export_requests: dict
        :type sourcedir: Path
        :type exportdir: Path
        :type args: Namespace
        """
        cache_info = {}
        if args.game_version.edition.media_cache:
            cache_info = load_media_cache(args.game_version.edition.media_cache)

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
                info("-- Exporting terrain files...")

            elif media_type is MediaType.GRAPHICS:
                kwargs["palettes"] = args.palettes
                kwargs["compression_level"] = args.compression_level
                kwargs["cache_info"] = cache_info
                export_func = MediaExporter._export_graphics
                info("-- Exporting graphics files...")

            elif media_type is MediaType.SOUNDS:
                kwargs["loglevel"] = args.debug_info
                kwargs["debugdir"] = args.debugdir
                export_func = MediaExporter._export_sound
                info("-- Exporting sound files...")

            elif media_type is MediaType.BLEND:
                kwargs["blend_mode_count"] = args.blend_mode_count
                export_func = MediaExporter._export_blend
                info("-- Exporting blend files...")

            total_count = len(cur_export_requests)
            for count, request in enumerate(cur_export_requests, start = 1):
                export_func(request, sourcedir, exportdir, **kwargs)
                print(f"-- Files done: {format_progress(count, total_count)}",
                      end = "\r", flush = True)

        if args.debug_info > 5:
            cachedata = {}
            for request in export_requests[MediaType.GRAPHICS]:
                kwargs = {}
                kwargs["palettes"] = args.palettes
                kwargs["compression_level"] = args.compression_level

                cache = MediaExporter._get_media_cache(
                    request,
                    sourcedir,
                    args.palettes,
                    compression_level=2
                )

                cachedata[request] = cache

            debug_info.debug_media_cache(
                args.debugdir,
                args.debug_info,
                sourcedir,
                cachedata,
                args.game_version
            )

    @staticmethod
    def _export_blend(
        export_request: MediaExportRequest,
        sourcedir: Path,
        exportdir: Path,
        blend_mode_count: int = None
    ) -> None:
        """
        Convert and export a blending mode.

        :param export_request: Export request for a blending mask.
        :param sourcedir: Directory where all media assets are mounted. Source subfolder and
                          source filename should be stored in the export request.
        :param exportdir: Directory the resulting file(s) will be exported to. Target subfolder
                          and target filename should be stored in the export request.
        :param blend_mode_count: Number of blending modes extracted from the source file.
        :type export_request: MediaExportRequest
        :type sourcedir: Path
        :type exportdir: Path
        :type blend_mode_count: int
        """
        source_file = sourcedir.joinpath(export_request.source_filename)

        media_file = source_file.open("rb")
        blend_data = Blendomatic(media_file, blend_mode_count)

        from .texture_merge import merge_frames

        textures = blend_data.get_textures()
        for idx, texture in enumerate(textures):
            merge_frames(texture)
            MediaExporter.save_png(
                texture,
                exportdir[export_request.targetdir],
                f"{export_request.target_filename}{idx}.png"
            )

            if get_loglevel() <= logging.DEBUG:
                MediaExporter.log_fileinfo(
                    source_file,
                    exportdir[export_request.targetdir,
                              f"{export_request.target_filename}{idx}.png"]
                )

    @staticmethod
    def _export_graphics(
        export_request: MediaExportRequest,
        sourcedir: Path,
        exportdir: Path,
        palettes: dict[int, ColorTable],
        compression_level: int,
        cache_info: dict = None
    ) -> None:
        """
        Convert and export a graphics file.

        :param export_request: Export request for a graphics file.
        :param sourcedir: Directory where all media assets are mounted. Source subfolder and
                          source filename should be stored in the export request.
        :param exportdir: Directory the resulting file(s) will be exported to. Target subfolder
                          and target filename should be stored in the export request.
        :param palettes: Palettes used by the game.
        :param compression_level: PNG compression level for the resulting image file.
        :param cache_info: Media cache information with compression parameters from a previous run.
        :type export_request: MediaExportRequest
        :type sourcedir: Path
        :type exportdir: Path
        :type palettes: dict
        :type compression_level: int
        :type cache_info: tuple
        """
        source_file = sourcedir[
            export_request.get_type().value,
            export_request.source_filename
        ]

        try:
            media_file = source_file.open("rb")

        except FileNotFoundError:
            if source_file.suffix.lower() in (".smx", ".sld"):
                # Rename extension to SMP and try again
                other_filename = export_request.source_filename[:-3] + "smp"
                source_file = sourcedir[
                    export_request.get_type().value,
                    other_filename
                ]
                export_request.set_source_filename(other_filename)

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

        elif source_file.suffix.lower() == ".sld":
            from ...value_object.read.media.sld import SLD
            image = SLD(media_file.read())

        else:
            raise SyntaxError(f"Source file {source_file.name} has an unrecognized extension: "
                              f"{source_file.suffix.lower()}")

        packer_cache = None
        compr_cache = None
        if cache_info:
            cache_params = cache_info.get(export_request.source_filename, None)

            if cache_params:
                packer_cache = cache_params["packer_settings"]
                compression_level = cache_params["compr_settings"][0]
                compr_cache = cache_params["compr_settings"][1:]

        from .texture_merge import merge_frames

        texture = Texture(image, palettes)
        merge_frames(texture, cache=packer_cache)
        MediaExporter.save_png(
            texture,
            exportdir[export_request.targetdir],
            export_request.target_filename,
            compression_level=compression_level,
            cache=compr_cache
        )
        metadata = {export_request.target_filename: texture.get_metadata()}
        export_request.set_changed()
        export_request.notify_observers(metadata)
        export_request.clear_changed()

        if get_loglevel() <= logging.DEBUG:
            MediaExporter.log_fileinfo(
                source_file,
                exportdir[export_request.targetdir, export_request.target_filename]
            )

    @staticmethod
    def _export_interface(
        export_request: MediaExportRequest,
        sourcedir: Path,
        **kwargs
    ) -> None:
        """
        Convert and export a sprite file.
        """
        # TODO: Implement

    @staticmethod
    def _export_palette(
        export_request: MediaExportRequest,
        sourcedir: Path,
        **kwargs
    ) -> None:
        """
        Convert and export a palette file.
        """
        # TODO: Implement

    @staticmethod
    def _export_sound(
        export_request: MediaExportRequest,
        sourcedir: Path,
        exportdir: Path,
        **kwargs
    ) -> None:
        """
        Convert and export a sound file.

        :param export_request: Export request for a sound file.
        :param sourcedir: Directory where all media assets are mounted. Source subfolder and
                          source filename should be stored in the export request.
        :param exportdir: Directory the resulting file(s) will be exported to. Target subfolder
                          and target filename should be stored in the export request.
        :type export_request: MediaExportRequest
        :type sourcedir: Path
        :type exportdir: Path
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
            debug_info.debug_not_found_sounds(kwargs["debugdir"], kwargs["loglevel"], source_file)
            return

        from ...service.export.opus.opusenc import encode

        soundata = encode(media_file)

        if isinstance(soundata, (str, int)):
            raise RuntimeError(f"opusenc failed: {soundata}")

        export_file = exportdir[
            export_request.targetdir,
            export_request.target_filename
        ]

        with export_file.open_w() as outfile:
            outfile.write(soundata)

        if get_loglevel() <= logging.DEBUG:
            MediaExporter.log_fileinfo(
                source_file,
                exportdir[export_request.targetdir, export_request.target_filename]
            )

    @staticmethod
    def _export_terrain(
        export_request: MediaExportRequest,
        sourcedir: Path,
        exportdir: Path,
        palettes: dict[int, ColorTable],
        game_version: GameVersion,
        compression_level: int
    ) -> None:
        """
        Convert and export a terrain graphics file.

        :param export_request: Export request for a terrain graphics file.
        :param sourcedir: Directory where all media assets are mounted. Source subfolder and
                          source filename should be stored in the export request.
        :param exportdir: Directory the resulting file(s) will be exported to. Target subfolder
                          and target filename should be stored in the export request.
        :param game_version: Game edition and expansion info.
        :param palettes: Palettes used by the game.
        :param compression_level: PNG compression level for the resulting image file.
        :type export_request: MediaExportRequest
        :type sourcedir: Directory
        :type exportdir: Directory
        :type palettes: dict
        :type game_version: GameVersion
        :type compression_level: int
        """
        source_file = sourcedir[
            export_request.get_type().value,
            export_request.source_filename
        ]

        if source_file.suffix.lower() == ".slp":
            from ...value_object.read.media.slp import SLP
            media_file = source_file.open("rb")
            image = SLP(media_file.read())

        elif source_file.suffix.lower() == ".dds":
            # TODO: Implement
            pass

        elif source_file.suffix.lower() == ".png":
            from shutil import copyfileobj
            src_path = source_file.open('rb')
            dst_path = exportdir[export_request.targetdir,
                                 export_request.target_filename].open('wb')
            copyfileobj(src_path, dst_path)
            return

        else:
            raise SyntaxError(f"Source file {source_file.name} has an unrecognized extension: "
                              f"{source_file.suffix.lower()}")

        if game_version.edition.game_id in ("AOC", "SWGB"):
            from .terrain_merge import merge_terrain
            texture = Texture(image, palettes)
            merge_terrain(texture)

        else:
            from .texture_merge import merge_frames
            texture = Texture(image, palettes)
            merge_frames(texture)

        MediaExporter.save_png(
            texture,
            exportdir[export_request.targetdir],
            export_request.target_filename,
            compression_level,
        )

        if get_loglevel() <= logging.DEBUG:
            MediaExporter.log_fileinfo(
                source_file,
                exportdir[export_request.targetdir, export_request.target_filename]
            )

    @staticmethod
    def _get_media_cache(
        export_request: MediaExportRequest,
        sourcedir: Path,
        palettes: dict[int, ColorTable],
        compression_level: int
    ) -> None:
        """
        Convert a media file and return the used settings. This performs
        a dry run, i.e. the graphics media is not saved on the filesystem.

        :param export_request: Export request for a graphics file.
        :param sourcedir: Directory where all media assets are mounted. Source subfolder and
                          source filename should be stored in the export request.
        :param exportdir: Directory the resulting file(s) will be exported to. Target subfolder
                          and target filename should be stored in the export request.
        :param palettes: Palettes used by the game.
        :param compression_level: PNG compression level for the resulting image file.
        :type export_request: MediaExportRequest
        :type sourcedir: Path
        :type exportdir: Path
        :type palettes: dict
        :type compression_level: int
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

        elif source_file.suffix.lower() == ".sld":
            from ...value_object.read.media.sld import SLD
            image = SLD(media_file.read())

        else:
            raise SyntaxError(f"Source file {source_file.name} has an unrecognized extension: "
                              f"{source_file.suffix.lower()}")

        from .texture_merge import merge_frames
        texture = Texture(image, palettes)
        merge_frames(texture)
        MediaExporter.save_png(
            texture,
            None,
            None,
            compression_level=compression_level,
            cache=None,
            dry_run=True
        )

        return texture.get_cache_params()

    @staticmethod
    def save_png(
        texture: Texture,
        targetdir: Path,
        filename: str,
        compression_level: int = 1,
        cache: dict = None,
        dry_run: bool = False
    ) -> None:
        """
        Store the image data into the target directory path,
        with given filename="dir/out.png".

        :param texture: Texture with an image atlas.
        :param targetdir: Directory where the image file is created.
        :param filename: Name of the resulting image file.
        :param compression_level: PNG compression level used for the resulting image file.
        :param dry_run: If True, create the PNG but don't save it as a file.
        :type texture: Texture
        :type targetdir: Directory
        :type filename: str
        :type compression_level: int
        :type dry_run: bool
        """
        from ...service.export.png import png_create

        compression_levels = {
            0: png_create.CompressionMethod.COMPR_NONE,
            1: png_create.CompressionMethod.COMPR_DEFAULT,
            2: png_create.CompressionMethod.COMPR_OPTI,
            3: png_create.CompressionMethod.COMPR_GREEDY,
            4: png_create.CompressionMethod.COMPR_AGGRESSIVE,
        }

        if not dry_run:
            _, ext = os.path.splitext(filename)

            # only allow png
            if ext != ".png":
                raise ValueError("Filename invalid, a texture must be saved"
                                 f"as '*.png', not '*.{ext}'")

        compression_method = compression_levels.get(
            compression_level,
            png_create.CompressionMethod.COMPR_DEFAULT
        )
        png_data, compr_params = png_create.save(
            texture.image_data.data,
            compression_method,
            cache
        )

        if not dry_run:
            with targetdir[filename].open("wb") as imagefile:
                imagefile.write(png_data)

        if compr_params:
            texture.best_compr = (compression_level, *compr_params)

    @staticmethod
    def log_fileinfo(
        source_file: Path,
        target_file: Path
    ) -> None:
        """
        Log source and target file information to the shell.
        """
        source_format = source_file.suffix[1:].upper()
        target_format = target_file.suffix[1:].upper()

        source_path = source_file.resolve_native_path()
        if source_path:
            source_size = os.path.getsize(source_path)

        else:
            with source_file.open('r') as src:
                src.seek(0, os.SEEK_END)
                source_size = src.tell()

        target_path = target_file.resolve_native_path()
        target_size = os.path.getsize(target_path)

        log = ("Converted: "
               f"{source_file.name} "
               f"({source_format}, {source_size}B) "
               f"-> {target_file.name} "
               f"({target_format}, {target_size}B | "
               f"{(target_size / source_size * 100) - 100:+.1f}%)")

        dbg(log)
