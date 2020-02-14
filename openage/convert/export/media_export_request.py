# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Specifies a request for a media resource that should be
converted and exported into a modpack.
"""
from openage.convert.dataformat.media_types import MediaType
from openage.convert.slp import SLP
from openage.util.fslike.path import Path
from openage.convert.colortable import ColorTable
from openage.convert.texture import Texture


class MediaExportRequest:

    def __init__(self, targetdir, source_filename, target_filename):
        """
        Create a request for a media file.

        :param targetdir: Relative path to the export directory.
        :type targetdir: str
        :param source_filename: Filename of the source file.
        :type source_filename: str
        :param target_filename: Filename of the resulting file.
        :type target_filename: str
        """

        self.set_targetdir(targetdir)
        self.set_source_filename(source_filename)
        self.set_target_filename(target_filename)

    def get_type(self):
        """
        Return the media type.
        """
        raise NotImplementedError("%s has not implemented get_type()"
                                  % (self))

    def save(self, sourcedir, exportdir):
        """
        Convert the media to openage target format and output the result
        to a file. Encountered metadata is returned on completion.

        :param sourcedir: Relative path to the source directory.
        :type sourcedir: ...util.fslike.path.Path
        :param exportdir: Relative path to the export directory.
        :type exportdir: ...util.fslike.path.Path
        """
        raise NotImplementedError("%s has not implemented save()"
                                  % (self))

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


class GraphicsMediaExportRequest(MediaExportRequest):
    """
    Export requests for ingame graphics such as animations or sprites.
    """

    def get_type(self):
        return MediaType.GRAPHICS

    def save(self, sourcedir, exportdir, palette_file):
        sourcefile = sourcedir.joinpath(self.source_filename)

        media_file = sourcefile.open("rb")
        palette_file = palette_file.open("rb")
        palette_table = ColorTable(palette_file.read())

        if sourcefile.suffix().lower() == "slp":
            from ..slp import SLP

            image = SLP(media_file.read())

        elif sourcefile.suffix().lower() == "smp":
            from ..smp import SMP

            image = SMP(media_file.read())

        elif sourcefile.suffix().lower() == "smx":
            from ..smx import SMX

            image = SMX(media_file.read())

        texture = Texture(image, palette_table)
        texture.save(self.targetdir, self.target_filename)
