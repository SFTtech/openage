# Copyright 2020-2022 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-few-public-methods
"""
Export data from a modpack to files.
"""
from __future__ import annotations
import typing


from ....log import info
from .data_exporter import DataExporter
from .generate_manifest_hashes import generate_hashes
from .media_exporter import MediaExporter

if typing.TYPE_CHECKING:
    from argparse import Namespace

    from openage.convert.entity_object.conversion.modpack import Modpack


class ModpackExporter:
    """
    Writes the contents of a created modpack into a targetdir.
    """

    @staticmethod
    def export(modpack: Modpack, args: Namespace) -> None:
        """
        Export a modpack to a directory.

        :param modpack: Modpack that is going to be exported.
        :param args: Converter arguments.
        :type modpack: ..dataformats.modpack.Modpack
        :type args: Namespace
        """
        sourcedir = args.srcdir
        exportdir = args.targetdir

        modpack_dir = exportdir.joinpath(f"{modpack.info.packagename}")

        info("Starting export...")
        info("Dumping info file...")

        # Modpack info file
        DataExporter.export([modpack.info], modpack_dir)

        info("Dumping data files...")

        # Data files
        DataExporter.export(modpack.get_data_files(), modpack_dir)

        if args.flag("no_media"):
            info("Skipping media file export...")
            return

        info("Exporting media files...")

        # Media files
        MediaExporter.export(modpack.get_media_files(), sourcedir, modpack_dir, args)

        info("Dumping metadata files...")

        # Metadata files
        DataExporter.export(modpack.get_metadata_files(), modpack_dir)

        # Manifest file
        generate_hashes(modpack, modpack_dir)
        DataExporter.export([modpack.manifest], modpack_dir)
