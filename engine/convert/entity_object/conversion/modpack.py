# Copyright 2020-2023 the openage authors. See copying.md for legal info.

"""
Defines a modpack that can be exported.
"""

from __future__ import annotations

from ..export.data_definition import DataDefinition
from ..export.formats.modpack_info import ModpackInfo
from ..export.formats.modpack_manifest import ManifestFile
from ..export.media_export_request import MediaExportRequest
from ..export.metadata_export import MetadataExport


class Modpack:
    """
    A collection of data and media files.
    """

    def __init__(self, name: str):

        self.name = name

        # Definition file
        self.info = ModpackInfo("", "modpack.toml")

        # Manifest file
        self.manifest = ManifestFile("", "manifest.toml")

        # Data/media export
        self.data_export_files: list[DataDefinition] = []
        self.media_export_files: list[MediaExportRequest] = {}
        self.metadata_files: list[MetadataExport] = []

    def add_data_export(self, export_file: DataDefinition) -> None:
        """
        Add a data file to the modpack for exporting.
        """
        if not isinstance(export_file, DataDefinition):
            raise TypeError(f"{repr(self)}: export file must be of type DataDefinition "
                            f"not {type(export_file)}")

        self.data_export_files.append(export_file)

    def add_media_export(self, export_request: MediaExportRequest) -> None:
        """
        Add a media export request to the modpack.
        """
        if not isinstance(export_request, MediaExportRequest):
            raise TypeError(f"{repr(self)}: export file must be of type MediaExportRequest "
                            f"not {type(export_request)}")

        if export_request.get_type() in self.media_export_files:
            self.media_export_files[export_request.get_type()].append(export_request)

        else:
            self.media_export_files[export_request.get_type()] = [export_request]

    def add_metadata_export(self, export_file: MetadataExport) -> None:
        """
        Add a metadata file to the modpack for exporting.
        """
        if not isinstance(export_file, MetadataExport):
            raise TypeError(f"{repr(self)}: export file must be of type MetadataExport "
                            f"not {type(export_file)}")

        self.metadata_files.append(export_file)

    def get_info(self) -> ModpackInfo:
        """
        Return the modpack definition file.
        """
        return self.info

    def get_data_files(self) -> list[DataDefinition]:
        """
        Returns the data files for exporting.
        """
        return self.data_export_files

    def get_media_files(self) -> list[MediaExportRequest]:
        """
        Returns the media requests for exporting.
        """
        return self.media_export_files

    def get_metadata_files(self) -> list[MetadataExport]:
        """
        Returns the metadata exports.
        """
        return self.metadata_files
