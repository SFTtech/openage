# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,arguments-differ
"""
Specifies a request for a media resource that should be
converted and exported into a modpack.
"""
from openage.convert.dataformat.version_detect import GameEdition

from ...util.observer import Observable
from ..dataformat.media_types import MediaType
from ..texture import Texture


class MediaExportRequest(Observable):
    """
    Generic superclass for export requests.
    """

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
        super().__init__()

        self.set_targetdir(targetdir)
        self.set_source_filename(source_filename)
        self.set_target_filename(target_filename)

    def get_type(self):
        """
        Return the media type.
        """
        raise NotImplementedError("%s has not implemented get_type()"
                                  % (self))

    def save(self, sourcedir, exportdir, *args, **kwargs):
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

    def save(self, sourcedir, exportdir, palettes, *args, **kwargs):
        source_file = sourcedir[self.get_type().value, self.source_filename]

        try:
            media_file = source_file.open("rb")

        except FileNotFoundError:
            if source_file.suffix.lower() == ".smx":
                # Rename extension to SMP and try again
                other_filename = self.source_filename[:-1] + "p"
                source_file = sourcedir[self.get_type().value, other_filename]

            media_file = source_file.open("rb")

        if source_file.suffix.lower() == ".slp":
            from ..slp import SLP
            image = SLP(media_file.read())

        elif source_file.suffix.lower() == ".smp":
            from ..smp import SMP
            image = SMP(media_file.read())

        elif source_file.suffix.lower() == ".smx":
            from ..smx import SMX
            image = SMX(media_file.read())

        texture = Texture(image, palettes)
        metadata = texture.save(exportdir.joinpath(self.targetdir), self.target_filename)
        metadata = {self.target_filename: metadata}

        self.set_changed()
        self.notify_observers(metadata)
        self.clear_changed()


class TerrainMediaExportRequest(MediaExportRequest):
    """
    Export requests for terrain graphics.
    """

    def get_type(self):
        return MediaType.TERRAIN

    def save(self, sourcedir, exportdir, palettes, game_version, *args, **kwargs):
        source_file = sourcedir[self.get_type().value, self.source_filename]
        media_file = source_file.open("rb")

        if source_file.suffix.lower() == ".slp":
            from ..slp import SLP
            image = SLP(media_file.read())

        elif source_file.suffix.lower() == ".dds":
            # TODO: Implement
            pass

        if game_version[0] is GameEdition.AOC:
            from ..texture import merge_terrain
            texture = Texture(image, palettes, custom_merger=merge_terrain)

        else:
            texture = Texture(image, palettes)
        texture.save(exportdir.joinpath(self.targetdir), self.target_filename)


class SoundMediaExportRequest(MediaExportRequest):
    """
    Export requests for ingame sounds.
    """

    def get_type(self):
        return MediaType.SOUNDS

    def save(self, sourcedir, exportdir, *args, **kwargs):
        source_file = sourcedir[self.get_type().value, self.source_filename]

        if source_file.is_file():
            with source_file.open_r() as infile:
                media_file = infile.read()

        else:
            # TODO: Filter files that do not exist out sooner
            return

        from ..opus.opusenc import encode

        soundata = encode(media_file)

        if isinstance(soundata, (str, int)):
            raise Exception("opusenc failed: {}".format(soundata))

        with exportdir[self.targetdir, self.target_filename].open_w() as outfile:
            outfile.write(soundata)
