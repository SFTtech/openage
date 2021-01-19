# Copyright 2020-2021 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,arguments-differ
"""
Specifies a request for a media resource that should be
converted and exported into a modpack.
"""

from ....util.observer import Observable


class MediaExportRequest(Observable):
    """
    Generic superclass for export requests.
    """

    __slots__ = ("media_type", "targetdir", "source_filename", "target_filename")

    def __init__(self, media_type, targetdir, source_filename, target_filename):
        """
        Create a request for a media file.

        :param targetdir: Relative path to the export directory.
        :type targetdir: str
        :param source_filename: Filename of the source file.
        :type source_filename: str
        :param target_filename: Filename of the resulting file.
        :type target_filename: str
        """
        super().__init__()

        self.media_type = media_type
        self.set_targetdir(targetdir)
        self.set_source_filename(source_filename)
        self.set_target_filename(target_filename)

    def get_type(self):
        """
        Return the media type.
        """
        return self.media_type

    def set_source_filename(self, filename):
        """
        Sets the filename for the source file.

        :param filename: Filename of the source file.
        :type filename: str
        """
        if not isinstance(filename, str):
            raise ValueError(f"str expected as source filename, not {type(filename)}")

        self.source_filename = filename

    def set_target_filename(self, filename):
        """
        Sets the filename for the target file.

        :param filename: Filename of the resulting file.
        :type filename: str
        """
        if not isinstance(filename, str):
            raise ValueError(f"str expected as target filename, not {type(filename)}")

        self.target_filename = filename

    def set_targetdir(self, targetdir):
        """
        Sets the target directory for the file.

        :param targetdir: Relative path to the export directory.
        :type targetdir: str
        """
        if not isinstance(targetdir, str):
            raise ValueError(f"str expected as targetdir, not {type(targetdir)}")

        self.targetdir = targetdir
