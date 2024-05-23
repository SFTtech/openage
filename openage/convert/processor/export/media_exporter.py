# Copyright 2021-2024 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-arguments,too-many-locals
"""
Converts media requested by export requests to files.
"""
from __future__ import annotations
import typing

import logging
import os
import multiprocessing
import queue
import sys

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
    from openage.util.dll import DllDirectoryManager


class MediaExporter:
    """
    Provides functions for converting media files and writing them to a targetdir.

    TODO: Avoid code duplication in the export functions.
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

            # Function for reading the source file data
            read_data_func = None

            # Multi-threaded function for exporting the source file data
            export_func = None

            # Optional function for handling data in the outqueue
            handle_outqueue_func = None

            kwargs = {}
            if media_type is MediaType.BLEND:
                read_data_func = MediaExporter._get_blend_data
                export_func = _export_blend
                itargs = (sourcedir, exportdir)
                kwargs["blend_mode_count"] = args.blend_mode_count
                info("-- Exporting blend files...")

            elif media_type is MediaType.GRAPHICS:
                read_data_func = MediaExporter._get_graphics_data
                export_func = _export_texture
                handle_outqueue_func = MediaExporter._handle_graphics_outqueue
                itargs = (args.palettes, args.compression_level)
                kwargs["cache_info"] = cache_info
                info("-- Exporting graphics files...")

            elif media_type is MediaType.SOUNDS:
                read_data_func = MediaExporter._get_sound_data
                export_func = _export_sound
                itargs = tuple()
                kwargs["debugdir"] = args.debugdir
                kwargs["loglevel"] = args.debug_info
                info("-- Exporting sound files...")

            elif media_type is MediaType.TERRAIN:
                read_data_func = MediaExporter._get_terrain_data
                export_func = _export_terrain
                itargs = (args.palettes, args.compression_level, args.game_version)
                info("-- Exporting terrain files...")

            if args.jobs == 1:
                MediaExporter._export_singlethreaded(
                    cur_export_requests,
                    sourcedir,
                    exportdir,
                    read_data_func,
                    export_func,
                    handle_outqueue_func,
                    itargs,
                    kwargs
                )

            else:
                MediaExporter._export_multithreaded(
                    cur_export_requests,
                    sourcedir,
                    exportdir,
                    read_data_func,
                    export_func,
                    handle_outqueue_func,
                    itargs,
                    kwargs,
                    args.jobs,
                    args.dll_manager,
                )

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
    def _export_singlethreaded(
        requests: list[MediaExportRequest],
        sourcedir: Path,
        exportdir: Path,
        read_data_func: typing.Callable,
        export_func: typing.Callable,
        handle_outqueue_func: typing.Callable | None,
        itargs: tuple,
        kwargs: dict
    ):
        """
        Export media files in a single thread.

        :param requests: Export requests for media files.
        :param sourcedir: Directory where all media assets are mounted. Source subfolder and
                          source filename should be stored in the export request.
        :param exportdir: Directory the resulting file(s) will be exported to. Target subfolder
                          and target filename should be stored in the export request.
        :param read_data_func: Function for reading the source file data.
        :param export_func: Function for exporting media files.
        :param handle_outqueue_func: Optional function for handling data in the outqueue.
        :param itargs: Arguments for the export function.
        :param kwargs: Keyword arguments for the export function.
        :type requests: list[MediaExportRequest]
        :type sourcedir: Path
        :type exportdir: Path
        :type read_data_func: typing.Callable
        :type export_func: typing.Callable
        :type handle_outqueue_func: typing.Callable
        :type itargs: tuple
        :type kwargs: dict
        """
        single_queue = queue.Queue()
        for idx, request in enumerate(requests):
            source_data = read_data_func(request, sourcedir, **kwargs)
            if source_data is None:
                continue

            target_path = exportdir[request.targetdir, request.target_filename]

            export_func(
                idx,
                source_data,
                single_queue,
                None,
                request.source_filename,
                target_path,
                *itargs,
                **kwargs
            )

            if get_loglevel() <= logging.DEBUG:
                MediaExporter.log_fileinfo(
                    sourcedir[request.get_type().value, request.source_filename],
                    exportdir[request.targetdir, request.target_filename]
                )

            MediaExporter._show_progress(idx + 1, len(requests))

        if handle_outqueue_func:
            handle_outqueue_func(single_queue, requests)

    @staticmethod
    def _export_multithreaded(
        requests: list[MediaExportRequest],
        sourcedir: Path,
        exportdir: Path,
        read_data_func: typing.Callable,
        export_func: typing.Callable,
        handle_outqueue_func: typing.Callable | None,
        itargs: tuple,
        kwargs: dict,
        job_count: int = None,
        dll_manager: DllDirectoryManager = None,
    ):
        """
        Export media files in multiple threads.

        :param requests: Export requests for media files.
        :param sourcedir: Directory where all media assets are mounted. Source subfolder and
                          source filename should be stored in the export request.
        :param exportdir: Directory the resulting file(s) will be exported to. Target subfolder
                          and target filename should be stored in the export request.
        :param read_data_func: Function for reading the source file data.
        :param export_func: Function for exporting media files.
        :param handle_outqueue_func: Optional function for handling data in the outqueue.
        :param itargs: Arguments for the export function.
        :param kwargs: Keyword arguments for the export function.
        :param job_count: Number of worker processes to use.
        :param dll_manager: Adds DLL search paths for the subrocesses (Windows-only).
        """
        worker_count = job_count
        if worker_count is None:
            # Small optimization that saves some time for small exports
            worker_count = min(multiprocessing.cpu_count(), len(requests))

        def error_callback(exception: Exception):
            """
            Error callback for the worker pool.
            """
            raise exception

        # Create a manager for sharing data between the workers and main process
        with multiprocessing.Manager() as manager:
            # Workers write the image metadata to this queue
            # so that it can be forwarded to the export requests
            #
            # we cannot do this in a worker process directly
            # because the export requests cannot be pickled
            outqueue = manager.Queue()

            expected_size = len(requests)

            # Create a pool of workers
            with multiprocessing.Pool(worker_count) as pool:
                for idx, request in enumerate(requests):
                    # Feed the worker with the source file data (bytes) from the
                    # main process
                    #
                    # This is necessary because some image files are inside an
                    # archive and cannot be accessed asynchronously
                    source_data = read_data_func(request, sourcedir, **kwargs)
                    if source_data is None:
                        expected_size -= 1
                        continue

                    target_path = exportdir[request.targetdir, request.target_filename]

                    # Start an export call in a worker process
                    # The call is asynchronous, so the next worker can be
                    # started immediately
                    pool.apply_async(
                        export_func,
                        args=(
                            idx,
                            source_data,
                            outqueue,
                            dll_manager,
                            request.source_filename,
                            target_path,
                            *itargs
                        ),
                        kwds=kwargs,
                        error_callback=error_callback
                    )

                    # Show progress
                    MediaExporter._show_progress(outqueue.qsize(), expected_size)

                # Close the pool since all workers have been started
                pool.close()

                # Show progress for remaining workers
                while outqueue.qsize() < expected_size:
                    MediaExporter._show_progress(outqueue.qsize(), expected_size)

                # Wait for all workers to finish
                pool.join()

            if handle_outqueue_func:
                handle_outqueue_func(outqueue, requests)

        # Log file information
        if get_loglevel() <= logging.DEBUG:
            for request in requests:
                MediaExporter.log_fileinfo(
                    sourcedir[request.get_type().value, request.source_filename],
                    exportdir[request.targetdir, request.target_filename]
                )

    @staticmethod
    def _get_blend_data(
        request: MediaExportRequest,
        sourcedir: Path,
        **kwargs  # pylint: disable=unused-argument
    ) -> bytes:
        """
        Get the raw file data of a blending mask.

        :param request: Export request for a blending mask.
        :param sourcedir: Directory where all media assets are mounted.
        :type request: MediaExportRequest
        :type sourcedir: Path
        """
        source_file = sourcedir[request.get_type().value,
                                request.source_filename]

        return source_file.open("rb").read()

    @staticmethod
    def _get_graphics_data(
        request: MediaExportRequest,
        sourcedir: Path,
        **kwargs  # pylint: disable=unused-argument
    ) -> bytes:
        """
        Get the raw file data of a graphics file.

        :param request: Export request for a graphics file.
        :param sourcedir: Directory where all media assets are mounted.
        :type request: MediaExportRequest
        :type sourcedir: Path
        """
        source_file = sourcedir[request.get_type().value,
                                request.source_filename]
        if not source_file.exists():
            if source_file.suffix.lower() in (".smx", ".sld"):
                # Some DE2 graphics files have the wrong extension
                # Fall back to the SMP (beta) extension
                other_filename = request.source_filename[:-3] + "smp"
                source_file = sourcedir[
                    request.get_type().value,
                    other_filename
                ]
                request.set_source_filename(other_filename)

        return source_file.open("rb").read()

    @staticmethod
    def _get_sound_data(
        request: MediaExportRequest,
        sourcedir: Path,
        **kwargs
    ) -> bytes | None:
        """
        Get the raw file data of a sound file.

        :param request: Export request for a sound file.
        :param sourcedir: Directory where all media assets are mounted.
        :type request: MediaExportRequest
        :type sourcedir: Path
        """
        source_file = sourcedir[request.get_type().value,
                                request.source_filename]

        if not source_file.is_file():
            # TODO: Filter files that do not exist out sooner
            debug_info.debug_not_found_sounds(kwargs["debugdir"],
                                              kwargs["loglevel"],
                                              source_file)
            return None

        return source_file.open("rb").read()

    @staticmethod
    def _get_terrain_data(
        request: MediaExportRequest,
        sourcedir: Path,
        **kwargs  # pylint: disable=unused-argument
    ) -> bytes:
        """
        Get the raw file data of a terrain graphics file.

        :param request: Export request for a terrain graphics file.
        :param sourcedir: Directory where all media assets are mounted.
        :type request: MediaExportRequest
        :type sourcedir: Path
        """
        source_file = sourcedir[request.get_type().value,
                                request.source_filename]

        return source_file.open("rb").read()

    @staticmethod
    def _handle_graphics_outqueue(
        outqueue: multiprocessing.Queue,
        requests: list[MediaExportRequest]
    ):
        """
        Collect the metadata from the workers and forward it to the
        export requests.

        This must be called before the manager of the queue is shutdown!

        :param outqueue: Queue for passing metadata to the main process.
        :param requests: Export requests for graphics files.
        :type outqueue: multiprocessing.Queue
        :type requests: list[MediaExportRequest]
        """
        while not outqueue.empty():
            idx, metadata = outqueue.get()
            update_data = {requests[idx].target_filename: metadata}
            requests[idx].set_changed()
            requests[idx].notify_observers(update_data)
            requests[idx].clear_changed()

    @staticmethod
    def _show_progress(
        current_size: int,
        total_size: int,
    ):
        """
        Show the progress of the export process.

        :param current_size: Number of files that have been exported.
        :param total_size: Total number of files to export.
        :type current_size: int
        :type total_size: int
        """
        print(f"-- Files done: {format_progress(current_size, total_size)}",
              end = "\r", flush = True)

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

        source_size = source_file.filesize
        target_size = target_file.filesize

        log = ("Converted: "
               f"{source_file.name} "
               f"({source_format}, {source_size}B) "
               f"-> {target_file.name} "
               f"({target_format}, {target_size}B | "
               f"{(target_size / source_size * 100) - 100:+.1f}%)")

        dbg(log)


def _export_blend(
    request_id: int,
    blendfile_data: bytes,
    outqueue: multiprocessing.Queue,
    dll_manager: DllDirectoryManager,
    source_filename: str,  # pylint: disable=unused-argument
    targetdir: Path,
    target_filename: str,
    blend_mode_count: int = None
) -> None:
    """
    Convert and export a blending mode.

    :param request_id: ID of the export request.
    :param blendfile_data: Raw file data of the blending mask.
    :param outqueue: Queue for passing metadata to the main process.
    :param dll_manager: Adds DLL search paths for the subrocesses (Windows-only).
    :param target_path: Path to the resulting image file.
    :param blend_mode_count: Number of blending modes extracted from the source file.
    """
    if sys.platform == "win32" and dll_manager is not None:
        dll_manager.add_directories()

    blend_data = Blendomatic(blendfile_data, blend_mode_count)

    from .texture_merge import merge_frames

    textures = blend_data.get_textures()
    for idx, texture in enumerate(textures):
        merge_frames(texture)
        _save_png(
            texture,
            targetdir.joinpath(f"{target_filename}_{idx}.png")
        )

    outqueue.put(request_id)


def _export_sound(
    request_id: int,
    sound_data: bytes,
    outqueue: multiprocessing.Queue,
    dll_manager: DllDirectoryManager,
    source_filename: str,  # pylint: disable=unused-argument
    target_path: Path,
    **kwargs  # pylint: disable=unused-argument
) -> None:
    """
    Convert and export a sound file.

    :param request_id: ID of the export request.
    :param sound_data: Raw file data of the sound file.
    :param outqueue: Queue for passing metadata to the main process.
    :param dll_manager: Adds DLL search paths for the subrocesses (Windows-only).
    :param target_path: Path to the resulting sound file.
    """
    if sys.platform == "win32" and dll_manager is not None:
        dll_manager.add_directories()

    from ...service.export.opus.opusenc import encode
    encoded = encode(sound_data)

    if isinstance(encoded, (str, int)):
        raise RuntimeError(f"opusenc failed: {encoded}")

    with target_path.open("wb") as outfile:
        outfile.write(encoded)

    outqueue.put(request_id)


def _export_terrain(
    request_id: int,
    graphics_data: bytes,
    outqueue: multiprocessing.Queue,
    dll_manager: DllDirectoryManager,
    source_filename: str,
    target_path: Path,
    palettes: dict[int, ColorTable],
    compression_level: int,
    game_version: GameVersion
) -> None:
    """
    Convert and export a terrain graphics file.

    :param request_id: ID of the export request.
    :param graphics_data: Raw file data of the graphics file.
    :param outqueue: Queue for passing the image metadata to the main process.
    :param dll_manager: Adds DLL search paths for the subrocesses (Windows-only).
    :param source_filename: Filename of the source file.
    :param target_path: Path to the resulting image file.
    :param palettes: Palettes used by the game.
    :param compression_level: PNG compression level for the resulting image file.
    :param game_version: Game edition and expansion info.
    """
    if sys.platform == "win32" and dll_manager is not None:
        dll_manager.add_directories()

    file_ext = source_filename.split('.')[-1].lower()
    if file_ext == "slp":
        from ...value_object.read.media.slp import SLP
        image = SLP(graphics_data)

    elif file_ext == "dds":
        # TODO: Implement
        pass

    elif file_ext == "png":
        with target_path.open("wb") as imagefile:
            imagefile.write(graphics_data)

        outqueue.put(request_id)
        return

    else:
        raise SyntaxError(f"Source file {source_filename} has an unrecognized extension: "
                          f"{file_ext}")

    if game_version.edition.game_id in ("AOC", "SWGB"):
        from .terrain_merge import merge_terrain
        texture = Texture(image, palettes)
        merge_terrain(texture)

    else:
        from .texture_merge import merge_frames
        texture = Texture(image, palettes)
        merge_frames(texture)

    _save_png(
        texture,
        target_path,
        compression_level=compression_level
    )

    outqueue.put(request_id)


def _export_texture(
    request_id: int,
    graphics_data: bytes,
    outqueue: multiprocessing.Queue,
    dll_manager: DllDirectoryManager,
    source_filename: str,
    target_path: Path,
    palettes: dict[int, ColorTable],
    compression_level: int,
    cache_info: dict = None
) -> None:
    """
    Convert and export a graphics file to a PNG texture.

    :param request_id: ID of the export request.
    :param graphics_data: Raw file data of the graphics file.
    :param outqueue: Queue for passing the image metadata to the main process.
    :param dll_manager: Adds DLL search paths for the subrocesses (Windows-only).
    :param source_filename: Filename of the source file.
    :param target_path: Path to the resulting image file.
    :param palettes: Palettes used by the game.
    :param compression_level: PNG compression level for the resulting image file.
    :param cache_info: Media cache information with compression parameters from a previous run.
    """
    if sys.platform == "win32" and dll_manager is not None:
        dll_manager.add_directories()

    file_ext = source_filename.split('.')[-1].lower()
    if file_ext == "slp":
        from ...value_object.read.media.slp import SLP
        image = SLP(graphics_data)

    elif file_ext == "smp":
        from ...value_object.read.media.smp import SMP
        image = SMP(graphics_data)

    elif file_ext == "smx":
        from ...value_object.read.media.smx import SMX
        image = SMX(graphics_data)

    elif file_ext == "sld":
        from ...value_object.read.media.sld import SLD
        image = SLD(graphics_data)

    else:
        raise SyntaxError(f"Source file {source_filename} has an unrecognized extension: "
                          f"{file_ext}")

    packer_cache = None
    compr_cache = None
    if cache_info:
        cache_params = cache_info.get(source_filename, None)

        if cache_params:
            packer_cache = cache_params["packer_settings"]
            compression_level = cache_params["compr_settings"][0]
            compr_cache = cache_params["compr_settings"][1:]

    from .texture_merge import merge_frames

    texture = Texture(image, palettes)
    merge_frames(texture, cache=packer_cache)
    _save_png(
        texture,
        target_path,
        compression_level=compression_level,
        cache=compr_cache
    )
    metadata = (request_id, texture.get_metadata().copy())
    outqueue.put(metadata)


def _save_png(
    texture: Texture,
    target_path: Path,
    compression_level: int = 1,
    cache: dict = None,
    dry_run: bool = False
) -> None:
    """
    Store the image data into the target directory path,
    with given target_path="dir/out.png".

    :param texture: Texture with an image atlas.
    :param target_path: Path to the resulting image file.
    :param compression_level: PNG compression level used for the resulting image file.
    :param dry_run: If True, create the PNG but don't save it as a file.
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
        ext = target_path.suffix.lower()

        # only allow png
        if ext != ".png":
            raise ValueError("Filename invalid, a texture must be saved"
                             f" as '*.png', not '*{ext}'")

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
        with target_path.open("wb") as imagefile:
            imagefile.write(png_data)

    if compr_params:
        texture.best_compr = (compression_level, *compr_params)
