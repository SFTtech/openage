# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Specifies a request for a media resource that should be
converted and exported into a modpack.
"""


class MediaExportRequest:

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

        self.media_type = media_type

        self.set_targetdir(targetdir)
        self.set_source_filename(source_filename)
        self.set_target_filename(target_filename)

    def get_type(self):
        """
        Return the media type.
        """
        return self.media_type

    def export(self, sourcedir, exportdir):
        """
        Convert the media to openage target format and output the result
        to a file.

        :param sourcedir: Relative path to the source directory.
        :type sourcedir: ...util.fslike.path.Path
        :param exportdir: Relative path to the export directory.
        :type exportdir: ...util.fslike.path.Path
        """
        # TODO: Depends on media type and source file type

    def set_source_filename(self, filename):
        """
        Sets the filename for the source file.

        :param filename: Filename of the source file.
        :type filename: str
        """
        if not isinstance(filename, str):
            raise ValueError("str expected as source filename, not %s" %
                             type(filename))

        self.source_filename = filename

    def set_target_filename(self, filename):
        """
        Sets the filename for the target file.

        :param filename: Filename of the resulting file.
        :type filename: str
        """
        if not isinstance(filename, str):
            raise ValueError("str expected as target filename, not %s" %
                             type(filename))

        self.target_filename = filename

    def set_targetdir(self, targetdir):
        """
        Sets the target directory for the file.

        :param targetdir: Relative path to the export directory.
        :type targetdir: str
        """
        if not isinstance(targetdir, str):
            raise ValueError("str expected as targetdir, not %s" %
                             type(targetdir))

        self.targetdir = targetdir
