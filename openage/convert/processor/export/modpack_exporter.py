# Copyright 2020-2021 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-few-public-methods
"""
Export data from a modpack to files.
"""

from openage.convert.processor.export.data_exporter import DataExporter
from openage.convert.processor.export.media_exporter import MediaExporter

from ....log import info
from ...value_object.read.media_types import MediaType
from .generate_manifest_hashes import generate_hashes


class ModpackExporter:
    """
    Writes the contents of a created modpack into a targetdir.
    """

    @staticmethod
    def export(modpack, args):
        """
        Export a modpack to a directory.

        :param modpack: Modpack that is going to be exported.
        :type modpack: ..dataformats.modpack.Modpack
        :param exportdir: Directory wheere modpacks are stored.
        :type exportdir: ...util.fslike.path.Path
        """
        sourcedir = args.srcdir
        exportdir = args.targetdir

        modpack_dir = exportdir.joinpath(f"{modpack.info.packagename}")

        info("Starting export...")
        info("Dumping info file...")

        # Modpack info file
        DataExporter.export([modpack.info], modpack_dir, args)

        info("Dumping data files...")

        # Data files
        DataExporter.export(modpack.get_data_files(), modpack_dir, args)

        if args.flag("no_media"):
            info("Skipping media file export...")
            return

        info("Exporting media files...")

        # Media files
        MediaExporter.export(modpack.get_media_files(), sourcedir, modpack_dir, args)

        info("Dumping metadata files...")

        # Metadata files
        DataExporter.export(modpack.get_metadata_files(), modpack_dir, args)

        # Manifest file
        generate_hashes(modpack, modpack_dir)
        DataExporter.export([modpack.manifest], modpack_dir, args)
