# Copyright 2014-2020 the openage authors. See copying.md for legal info.

""" Routines for texture generation etc """

# TODO pylint: disable=C,R

from PIL import Image
import os

import numpy

from ....log import spam
from ....util.fslike.path import Path
from ...deprecated import struct_definition
from ...value_object.read.media.blendomatic import BlendingMode
from ...value_object.read.media.hardcoded.terrain_tile_size import TILE_HALFSIZE
from ..conversion import genie_structure


class TextureImage:
    """
    represents a image created from a (r,g,b,a) matrix.
    """

    def __init__(self, picture_data, hotspot=None):

        if isinstance(picture_data, Image.Image):
            if picture_data.mode != 'RGBA':
                picture_data = picture_data.convert('RGBA')

            picture_data = numpy.array(picture_data)

        if not isinstance(picture_data, numpy.ndarray):
            raise ValueError("Texture image must be created from PIL Image "
                             "or numpy array, not '%s'" % type(picture_data))

        self.width = picture_data.shape[1]
        self.height = picture_data.shape[0]

        spam("creating TextureImage with size %d x %d", self.width, self.height)

        if hotspot is None:
            self.hotspot = (0, 0)
        else:
            self.hotspot = hotspot

        self.data = picture_data

    def get_pil_image(self):
        return Image.fromarray(self.data)

    def get_data(self):
        return self.data


class Texture(genie_structure.GenieStructure):
    image_format = "png"

    name_struct = "subtexture"
    name_struct_file = "texture"
    struct_description = (
        "one sprite, as part of a texture atlas.\n"
        "\n"
        "this struct stores information about positions and sizes\n"
        "of sprites included in the 'big texture'."
    )

    def __init__(self, input_data, palettes=None, custom_cutter=None, custom_merger=False):
        super().__init__()
        spam("creating Texture from %s", repr(input_data))

        from ...value_object.read.media.slp import SLP
        from ...value_object.read.media.smp import SMP
        from ...value_object.read.media.smx import SMX

        if isinstance(input_data, (SLP, SMP, SMX)):
            frames = []

            for frame in input_data.main_frames:
                # Palette can be different for every frame
                main_palette = palettes[frame.get_palette_number()]
                for subtex in self._slp_to_subtextures(frame,
                                                       main_palette,
                                                       custom_cutter):
                    frames.append(subtex)

        elif isinstance(input_data, BlendingMode):
            frames = [
                # the hotspot is in the west corner of a tile.
                TextureImage(
                    tile.get_picture_data(),
                    hotspot=(0, TILE_HALFSIZE["y"])
                )
                for tile in input_data.alphamasks
            ]
        else:
            raise Exception("cannot create Texture "
                            "from unknown source type: %s" % (type(input_data)))

        if custom_merger:
            self.image_data, (self.width, self.height), self.image_metadata\
                = custom_merger(frames)

        else:
            from ...processor.export.texture_merge import merge_frames
            self.image_data, (self.width, self.height), self.image_metadata\
                = merge_frames(frames)

    def _slp_to_subtextures(self, frame, main_palette, custom_cutter=None):
        """
        convert slp to subtexture or subtextures, using a palette.
        """
        subtex = TextureImage(
            frame.get_picture_data(main_palette),
            hotspot=frame.get_hotspot()
        )

        if custom_cutter:
            # this may cut the texture into some parts
            return custom_cutter.cut(subtex)
        else:
            return [subtex]

    def save(self, targetdir, filename, compression_level=1):
        """
        Store the image data into the target directory path,
        with given filename="dir/out.png".

        :param compression_level: Compression level of the PNG. A higher
                                  level results in smaller file sizes, but
                                  takes longer to generate.
                                      - 0 = no compression
                                      - 1 = normal png compression (default)
                                      - 2 = greedy search for smallest file; slowdown is 8x
                                      - 3 = maximum possible compression; slowdown is 256x
        :type compression_level: int
        """
        from ...service.export.png import png_create

        COMPRESSION_LEVELS = {
            0: png_create.CompressionMethod.COMPR_NONE,
            1: png_create.CompressionMethod.COMPR_DEFAULT,
            2: png_create.CompressionMethod.COMPR_GREEDY,
            3: png_create.CompressionMethod.COMPR_AGGRESSIVE,
        }

        if not isinstance(targetdir, Path):
            raise ValueError("util.fslike Path expected as targetdir")
        if not isinstance(filename, str):
            raise ValueError("str expected as filename, not %s" %
                             type(filename))

        _, ext = os.path.splitext(filename)

        # only allow png
        if ext != ".png":
            raise ValueError("Filename invalid, a texture must be saved"
                             "as 'filename.png', not '%s'" % (filename))

        # without the dot
        ext = ext[1:]

        compression_method = COMPRESSION_LEVELS.get(compression_level, png_create.CompressionMethod.COMPR_DEFAULT)
        with targetdir[filename].open("wb") as imagefile:
            png_data, _ = png_create.save(self.image_data.data, compression_method)
            imagefile.write(png_data)

        return self.image_metadata

    @classmethod
    def structs(cls):
        return [struct_definition.StructDefinition(cls)]

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = (
            (True, "x", None, "int32_t"),
            (True, "y", None, "int32_t"),
            (True, "w", None, "int32_t"),
            (True, "h", None, "int32_t"),
            (True, "cx", None, "int32_t"),
            (True, "cy", None, "int32_t"),
        )
        return data_format
