# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Export data from a modpack to files.
"""
from openage.convert import game_versions
from openage.convert.dataformat.media_types import MediaType

from ...log import info


class ModpackExporter:

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
        game_version = args.game_version

        modpack_dir = exportdir.joinpath("%s" % (modpack.info.name))

        info("Starting export...")
        info("Dumping info file...")

        # Modpack info file
        modpack.info.save(modpack_dir)

        info("Dumping data files...")

        # Data files
        data_files = modpack.get_data_files()

        for data_file in data_files:
            data_file.save(modpack_dir)

        if args.flag("no_media"):
            info("Skipping media file export...")
            return

        info("Exporting media files...")

        # Media files
        media_files = modpack.get_media_files()

        for media_type in media_files.keys():
            cur_export_requests = media_files[media_type]

            for request in cur_export_requests:
                request.save(sourcedir, modpack_dir, game_version)

        info("Dumping metadata files...")

        # Metadata files
        metadata_files = modpack.get_metadata_files()

        for metadata_file in metadata_files:
            metadata_file.save(modpack_dir)
