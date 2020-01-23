# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Export data from a modpack to files.
"""


class ModpackExporter:

    @staticmethod
    def export(modpack, exportdir):
        """
        Export a modpack to a directory.

        :param modpack: Modpack that is going to be exported.
        :type modpack: ..dataformats.modpack.Modpack
        :param exportdir: Directory wheere modpacks are stored.
        :type exportdir: ...util.fslike.path.Path
        """
        modpack_dir = exportdir.joinpath("%s" % (modpack.info.name))

        # Modpack info file
        modpack.info.save(modpack_dir)

        # Data files
        data_files = modpack.get_data_files()

        for data_file in data_files:
            data_file.save(modpack_dir)

        # Graphics files
        graphics_files = modpack.get_graphics_files()

        for graphics_file in graphics_files:
            graphics_file.save(modpack_dir)

        # Sound files
        sound_files = modpack.get_sound_files()

        for sound_file in sound_files:
            sound_file.save(modpack_dir)
