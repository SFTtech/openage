# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-few-public-methods

"""
Export data from a modpack to files.
"""
from ....log import info
from ...value_object.read.media_types import MediaType


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

            kwargs = {}

            if media_type in (MediaType.GRAPHICS, MediaType.TERRAIN):
                # Game version and palettes
                kwargs["game_version"] = args.game_version
                kwargs["palettes"] = args.palettes

            for request in cur_export_requests:
                request.save(sourcedir, modpack_dir, **kwargs)

        info("Dumping metadata files...")

        # Metadata files
        metadata_files = modpack.get_metadata_files()

        for metadata_file in metadata_files:
            metadata_file.save(modpack_dir)
