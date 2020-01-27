# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Defines a modpack that can be exported.
"""

from openage.convert.export.formats.modpack_info import ModpackInfo
from openage.convert.export.data_definition import DataDefinition
from openage.convert.export.media_export_request import MediaExportRequest


class Modpack:

    def __init__(self, name):

        self.name = name

        # Definition file
        self.info = ModpackInfo("", self.name + ".mod", self.name)

        # Data/media export
        self.data_export_files = []
        self.media_export_files = {}

    def add_data_export(self, export_file):
        """
        Add a data file to the modpack for exporting.
        """
        if not isinstance(export_file, DataDefinition):
            raise Exception("%s: export file must be of type DataDefinition"
                            "not %s" % (self, type(export_file)))

        self.data_export_files.append(export_file)

    def add_media_export(self, export_request):
        """
        Add a media export request to the modpack.
        """
        if not isinstance(export_request, MediaExportRequest):
            raise Exception("%s: export file must be of type MediaExportRequest"
                            "not %s" % (self, type(export_request)))

        if export_request.get_media_type() in self.media_export_files.keys():
            self.media_export_files[export_request.get_media_type()].append(export_request)

        else:
            self.media_export_files[export_request.get_media_type()] = [export_request]

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

    def get_media_files(self):
        """
        Returns the media requests for exporting.
        """
        return self.media_export_files
