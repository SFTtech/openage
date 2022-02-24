# Copyright 2020-2022 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,arguments-differ
"""
Specifies a request for a media resource that should be
converted and exported into a modpack.
"""
from __future__ import annotations
import typing

from ....util.observer import Observable

if typing.TYPE_CHECKING:
    from openage.convert.value_object.read.media_types import MediaType


class MediaExportRequest(Observable):
    """
    Generic superclass for export requests.
    """

    __slots__ = ("media_type", "targetdir", "source_filename", "target_filename")

    def __init__(
        self,
        media_type: MediaType,
        targetdir: str,
        source_filename: str,
        target_filename: str
    ):
        """
        Create a request for a media file.

        :param media_type: Media type of the requested  source file.
        :type media_type: MediaType
        :param targetdir: Relative path to the export directory.
        :type targetdir: str
        :param source_filename: Filename of the source file.
        :type source_filename: str
        :param target_filename: Filename of the resulting file.
        :type target_filename: str
        """
        super().__init__()

        self.media_type = media_type
        self.targetdir = targetdir
        self.source_filename = source_filename
        self.target_filename = target_filename

    def get_type(self) -> MediaType:
        """
        Return the media type.
        """
        return self.media_type

    def set_source_filename(self, filename: str) -> None:
        """
        Sets the filename for the source file.

        :param filename: Filename of the source file.
        :type filename: str
        """
        if not isinstance(filename, str):
            raise ValueError(f"str expected as source filename, not {type(filename)}")

        self.source_filename = filename

    def set_target_filename(self, filename: str) -> None:
        """
        Sets the filename for the target file.

        :param filename: Filename of the resulting file.
        :type filename: str
        """
        if not isinstance(filename, str):
            raise ValueError(f"str expected as target filename, not {type(filename)}")

        self.target_filename = filename

    def set_targetdir(self, targetdir: str) -> None:
        """
        Sets the target directory for the file.

        :param targetdir: Relative path to the export directory.
        :type targetdir: str
        """
        if not isinstance(targetdir, str):
            raise ValueError(f"str expected as targetdir, not {type(targetdir)}")

        self.targetdir = targetdir
