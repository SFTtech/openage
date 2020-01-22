# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Defines a modpack that can be exported.
"""

from openage.convert.export.formats.modpack_info import ModpackInfo
from openage.convert.export.data_definition import DataDefinition


class Modpack:

    def __init__(self, name):

        self.name = name

        # Definition file
        self.info = ModpackInfo("", self.name + ".mod", self.name)

        # Data/media export
        self.data_export_files = []
        self.graphics_export_files = []
        self.sound_export_files = []

    def add_data_export(self, export_file):
        """
        Add a data file to the modpack for exporting.
        """
        if not isinstance(export_file, DataDefinition):
            raise Exception("%s: export file must be of type DataDefinition"
                            "not %s" % (self, type(export_file)))

        self.data_export_files.append(export_file)

    def add_graphics_export(self, export_file):
        """
        Add a graphics file to the modpack for exporting.
        """
        self.graphics_export_files.append(export_file)

    def add_sound_export(self, export_file):
        """
        Add a sound file to the modpack for exporting.
        """
        self.sound_export_files.append(export_file)

    def get_info(self):
        """
        Return the modpack definition file.
        """
        return self.info

    def get_data_files(self):
        """
        Returns the data files for exporting.
        """
        return self.data_export_files

    def get_graphics_files(self):
        """
        Returns the data files for exporting.
        """
        return self.graphics_export_files

    def get_sound_files(self):
        """
        Returns the data files for exporting.
        """
        return self.sound_export_files
