# Copyright 2014-2020 the openage authors. See copying.md for legal info.

""" Routines for texture generation etc """

# TODO pylint: disable=C,R

from PIL import Image
import math
import os

import numpy

from ..log import spam
from ..util.fslike.path import Path
from .binpack import RowPacker, ColumnPacker, BinaryTreePacker, BestPacker
from .dataformat import genie_structure
from .export import struct_definition
from .hardcoded.terrain_tile_size import TILE_HALFSIZE
from .hardcoded.texture import (MAX_TEXTURE_DIMENSION, MARGIN,
                                TERRAIN_ASPECT_RATIO)
from .value_object.media.blendomatic import BlendingMode


# REFA: function -> entity object
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


# REFA: function -> entity object
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

        from .value_object.media.slp import SLP
        from .value_object.media.smp import SMP
        from .value_object.media.smx import SMX

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

        from .png import png_create

        compression_method = png_create.CompressionMethod.COMPR_DEFAULT
        if compression_level == 0:
            pass

        elif compression_level == 2:
            compression_method = png_create.CompressionMethod.COMPR_GREEDY

        elif compression_level == 3:
            compression_method = png_create.CompressionMethod.COMPR_AGGRESSIVE

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


# REFA: function -> processor
def merge_frames(frames, custom_packer=None):
    """
    merge all given frames of this slp to a single image file.

    frames = [TextureImage, ...]

    returns = TextureImage, (width, height), [drawn_frames_meta]
    """

    if len(frames) == 0:
        raise Exception("cannot create texture with empty input frame list")

    if custom_packer:
        packer = custom_packer

    else:
        packer = BestPacker([BinaryTreePacker(margin=MARGIN, aspect_ratio=1),
                             BinaryTreePacker(margin=MARGIN,
                                              aspect_ratio=TERRAIN_ASPECT_RATIO),
                             RowPacker(margin=MARGIN),
                             ColumnPacker(margin=MARGIN)])

    packer.pack(frames)

    width, height = packer.width(), packer.height()
    assert width <= MAX_TEXTURE_DIMENSION, "Texture width limit exceeded"
    assert height <= MAX_TEXTURE_DIMENSION, "Texture height limit exceeded"

    area = sum(block.width * block.height for block in frames)
    used_area = width * height
    efficiency = area / used_area

    spam("merging %d frames to %dx%d atlas, efficiency %.3f.",
         len(frames), width, height, efficiency)

    atlas_data = numpy.zeros((height, width, 4), dtype=numpy.uint8)
    drawn_frames_meta = []

    for sub_frame in frames:
        sub_w = sub_frame.width
        sub_h = sub_frame.height

        pos_x, pos_y = packer.pos(sub_frame)

        spam("drawing frame %03d on atlas at %d x %d...",
             len(drawn_frames_meta), pos_x, pos_y)

        # draw the subtexture on atlas_data
        atlas_data[pos_y:pos_y + sub_h, pos_x:pos_x + sub_w] = sub_frame.data

        hotspot_x, hotspot_y = sub_frame.hotspot

        # generate subtexture meta information dict:
        # origin x, origin y, width, height, hotspot x, hotspot y
        drawn_frames_meta.append(
            {
                "x":  pos_x,
                "y":  pos_y,
                "w":  sub_w,
                "h":  sub_h,
                "cx": hotspot_x,
                "cy": hotspot_y,
            }
        )

    atlas = TextureImage(atlas_data)

    spam("successfully merged %d frames to atlas.", len(frames))

    return atlas, (width, height), drawn_frames_meta


# REFA: function -> processor
def merge_terrain(frames):
    """
    Merges tiles from an AoC terrain SLP into a single flat texture.

    You can imagine every terrain tile (frame) as a puzzle piece
    of the big merged terrain. By blending and overlapping
    the tiles, we create a single terrain texture.

    :param frames: Terrain tiles
    :type frames: TextureImage
    :returns: Resulting texture as well as width/height.
    :rtype: TextureImage, (width, height)
    """
    # Can be 10 (regular terrain) or 6 (farms)
    tiles_per_row = int(math.sqrt(len(frames)))

    # Size of one tile should be (98,49)
    frame_width = frames[0].width
    frame_height = frames[0].height

    half_offset_x = frame_width // 2
    half_offset_y = frame_height // 2

    merge_atlas_width = (frame_width * tiles_per_row) - (tiles_per_row - 1)
    merge_atlas_height = (frame_height * tiles_per_row) - (tiles_per_row - 1)

    merge_atlas = numpy.zeros((merge_atlas_height, merge_atlas_width, 4), dtype=numpy.uint8)

    index = 0
    for sub_frame in frames:
        tenth_frame = index // tiles_per_row
        every_frame = index % tiles_per_row

        # Offset of every terrain tile in relation to (0,0)
        # Tiles are shifted by the distance of a half tile
        # and blended into each other.
        merge_offset_x = ((every_frame * (half_offset_x))
                          + (tenth_frame * (half_offset_x)))
        merge_offset_y = ((merge_atlas_height // 2) - half_offset_y
                          - (every_frame * (half_offset_y))
                          + (tenth_frame * (half_offset_y)))

        # Iterate through every pixel in the frame and copy
        # colored pixels to the correct position in the merge image
        merge_coord_x = merge_offset_x
        merge_coord_y = merge_offset_y
        for frame_x in range(frame_width):
            for frame_y in range(frame_height):
                # Do an alpha blend:
                # this if skips all fully transparent pixels
                # which means we only copy colored pixels
                if numpy.any(sub_frame.data[frame_y][frame_x]):
                    merge_atlas[merge_coord_y][merge_coord_x] = sub_frame.data[frame_y][frame_x]

                merge_coord_y += 1

            merge_coord_x += 1
            merge_coord_y = merge_offset_y

        index += 1

    # Transform to a flat texture
    flat_atlas_width = merge_atlas_width // 2 + 1

    flat_atlas = numpy.zeros((merge_atlas_height, flat_atlas_width, 4), dtype=numpy.uint8)

    # Does a matrix transformation using
    # [  1 , -1  ]
    # [ 0.5, 0.5 ]
    # as the multipication matrix.
    # This reverses the dimetric projection (diamond shape view)
    # to a plan projection (bird's eye view).
    # Reference: https://gamedev.stackexchange.com/questions/
    #            16746/what-is-the-name-of-perspective-of-age-of-empires-ii
    for flat_x in range(flat_atlas_width):
        for flat_y in range(merge_atlas_height):
            merge_x = (1 * flat_x + flat_atlas_width - 1) - 1 * flat_y
            merge_y = math.floor(0.5 * flat_x + 0.5 * flat_y)

            if flat_x + flat_y < merge_atlas_height:
                merge_y = math.ceil(0.5 * flat_x + 0.5 * flat_y)

            flat_atlas[flat_y][flat_x] = merge_atlas[merge_y][merge_x]

    # Rotate by 270 degrees to match the rotation of HD terrain textures
    flat_atlas = numpy.ascontiguousarray(numpy.rot90(flat_atlas, 3, axes=(0, 1)))

    atlas = TextureImage(flat_atlas)

    return atlas, (atlas.width, atlas.height), None
