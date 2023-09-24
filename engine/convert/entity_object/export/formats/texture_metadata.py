# Copyright 2021-2021 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-arguments

"""
Texture definition file.
"""

from enum import Enum

from ..data_definition import DataDefinition

FORMAT_VERSION = '1'


class LayerMode(Enum):
    """
    Possible values for the mode of a layer.
    """
    OFF = 'off'     # layer is not animated
    ONCE = 'once'   # animation plays once
    LOOP = 'loop'   # animation loops indefinitely


class TextureMetadata(DataDefinition):
    """
    Collects texture metadata and can format it
    as a .texture custom format
    """

    def __init__(self, targetdir, filename):
        super().__init__(targetdir, filename)

        self.image_file = None
        self.size = {}
        self.pxformat = {}
        self.subtexs = []

    def add_subtex(self, xpos, ypos, xsize, ysize, xhotspot, yhotspot):
        """
        Add a subtex with all its spacial information.

        :param xpos: X position of the subtex on the image canvas.
        :type xpos: int
        :param ypos: Y position of the subtex on the image canvas.
        :type ypos: int
        :param xsize: Width of the subtex.
        :type xsize: int
        :param ysize: Height of the subtex.
        :type ysize: int
        :param xhotspot: X position of the hotspot of the subtex.
        :type xhotspot: int
        :param yhotspot: Y position of the hotspot of the subtex.
        :type yhotspot: int
        """
        self.subtexs.append(
            {
                "xpos": xpos,
                "ypos": ypos,
                "xsize": xsize,
                "ysize": ysize,
                "xhotspot": xhotspot,
                "yhotspot": yhotspot,
            }
        )

    def set_imagefile(self, filename):
        """
        Set the relative filename of the texture.

        :param filename: Path to the image file.
        :type filename: str
        """
        self.image_file = filename

    def set_size(self, width, height):
        """
        Define the size of the PNG file.

        :param width: Width of the exported PNG in pixels.
        :type width: int
        :param height: Height of the exported PNG in pixels.
        :type height: int
        """
        self.size = {
            "width": width,
            "height": height
        }

    def set_pxformat(self, pxformat="rgba8", cbits=True):
        """
        Specify the pixel format of the texture.

        :param pxformat: Identifier for the pixel format of each pixel.
        :type pxformat: str
        :param cbits: True if the pixels use a command bit.
        :type cbits: bool
        """
        self.pxformat = {
            "format": pxformat,
            "cbits": cbits,
        }

    def dump(self):
        output_str = ""

        # header
        output_str += "# openage texture definition file\n\n"

        # version
        output_str += f"version {FORMAT_VERSION}\n\n"

        # image file
        output_str += f"imagefile \"{self.image_file}\"\n"

        output_str += "\n"

        # size
        output_str += f"size {self.size['width']} {self.size['height']}\n"

        output_str += "\n"

        # pixel format
        output_str += f"pxformat {self.pxformat['format']}"

        if self.pxformat["cbits"]:
            output_str += f" cbits={self.pxformat['cbits']}"

        output_str += "\n\n"

        # subtex definitions
        for subtex in self.subtexs:
            output_str += f'subtex {" ".join(str(param) for param in subtex.values())}\n'

        return output_str

    def __repr__(self):
        return f'TextureMetadata<{self.filename}>'
