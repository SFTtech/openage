# Copyright 2014-2016 the openage authors. See copying.md for legal info.

""" Routines for texture generation etc """

# TODO pylint: disable=C,R


from .binpack import RowPacker, ColumnPacker, BinaryTreePacker, BestPacker
from .blendomatic import BlendingMode
from .dataformat import (exportable, data_definition,
                         struct_definition, data_formatter)
from .hardcoded.terrain_tile_size import TILE_HALFSIZE
from .hardcoded.texture import (MAX_TEXTURE_DIMENSION, MARGIN,
                                TERRAIN_ASPECT_RATIO)
from ..log import spam
from ..util.fslike.path import Path


def subtexture_meta(tx, ty, hx, hy, cx, cy):
    """
    generate a dict that contains the meta information for
    the given parameters:
        origin x, y
        height, width
        center/hotspot x, y
    """
    ret = {
        "x":  tx,
        "y":  ty,
        "w":  hx,
        "h":  hy,
        "cx": cx,
        "cy": cy,
    }

    return ret


class TextureImage:
    """
    represents a image created from a (r,g,b,a) matrix.
    """

    def __init__(self, picture_data, hotspot=None):

        from PIL import Image
        import numpy
        if isinstance(picture_data, Image.Image):
            picture_data = numpy.array(picture_data.getdata(), numpy.uint8).reshape(picture_data.size[1], picture_data.size[0], 4)

        self.width = picture_data.shape[1]
        self.height = picture_data.shape[0]

        spam("creating TextureImage with size %d x %d" % (
            self.width, self.height))

        if hotspot is None:
            self.hotspot = (0, 0)
        else:
            self.hotspot = hotspot

        self.data = picture_data

    def get_pil_image(self):
        from PIL import Image
        return Image.fromarray(self.data)


class Texture(exportable.Exportable):
    image_format = "png"

    name_struct = "subtexture"
    name_struct_file = "texture"
    struct_description = (
        "one sprite, as part of a texture atlas.\n"
        "\n"
        "this struct stores information about positions and sizes\n"
        "of sprites included in the 'big texture'."
    )

    data_format = (
        (True, "x", "int32_t"),
        (True, "y", "int32_t"),
        (True, "w", "int32_t"),
        (True, "h", "int32_t"),
        (True, "cx", "int32_t"),
        (True, "cy", "int32_t"),
    )

    # player-specific colors will be in color blue, but with an alpha of 254
    player_id = 1

    def __init__(self, input_data, palette=None, custom_cutter=None):
        super().__init__()
        spam("creating Texture from %s" % (repr(input_data)))

        from .slp import SLP

        if isinstance(input_data, SLP):
            if palette is None:
                raise Exception("palette needed for SLP -> texture generation")

            frames = []

            for frame in input_data.frames:
                for subtex in self.__slp_to_subtextures(frame, palette, custom_cutter):
                    frames.append(subtex)

        elif isinstance(input_data, BlendingMode):
            frames = [
                TextureImage(
                    tile.get_picture_data(),
                    hotspot=(TILE_HALFSIZE["x"], TILE_HALFSIZE["y"])
                )
                for tile in input_data.alphamasks
            ]
        else:
            raise Exception("cannot create Texture "
                            "from unknown source type: %s" % (type(input_data)))

        self.image_data, (self.width, self.height), self.image_metadata\
            = merge_frames(frames)

    def __slp_to_subtextures(self, frame, palette=None, custom_cutter=None):
        """
        convert slp to subtexture or subtextures, use a palette.
        """
        # TODO this needs some _serious_ performance work
        # (at least a 10x improvement, 50x would be better).
        # ideas: remove PIL and use libpng via CPPInterface,
        #        cythonize parts of SLP.py
        subtex = TextureImage(frame.get_picture_data(palette, self.player_id), hotspot=frame.info.hotspot)

        if custom_cutter:
            return custom_cutter(subtex)
        else:
            return [subtex]

    def save(self, targetdir, filename, meta_formats=None):
        """
        save the texture png and csv to the given path in obj.
        """
        if not isinstance(targetdir, Path):
            raise ValueError("util.fslike Path expected as targetdir")

        # generate PNG file
        with targetdir[filename + ".png"].open("wb") as imagefile:
            self.image_data.get_pil_image().save(imagefile, 'png')

        if meta_formats:
            # generate formatted texture metadata
            formatter = data_formatter.DataFormatter()
            formatter.add_data(self.dump(filename))
            formatter.export(targetdir, meta_formats)

    def dump(self, filename):
        return [data_definition.DataDefinition(self,
                                               self.image_metadata,
                                               filename)]

    @classmethod
    def structs(cls):
        return [struct_definition.StructDefinition(cls)]


def merge_frames(frames):
    """
    merge all given frames of this slp to a single image file.

    frames = [TextureImage, ...]

    returns = TextureImage, (width, height), [drawn_frames_meta]
    """

    import numpy

    if len(frames) == 0:
        raise Exception("cannot create texture with empty input frame list")

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

    spam("merging %d frames to %dx%d atlas, efficiency %.3f." %
         (len(frames), width, height, efficiency))

    atlas_data = numpy.zeros((height, width, 4), dtype=numpy.uint8)
    drawn_frames_meta = []

    for sub_frame in frames:
        sub_w = sub_frame.width
        sub_h = sub_frame.height

        pos_x, pos_y = packer.pos(sub_frame)

        spam("drawing frame %03d on atlas at %d x %d..." % (
            len(drawn_frames_meta), pos_x, pos_y))

        # draw the subtexture on atlas_data
        atlas_data[pos_y:pos_y + sub_h, pos_x:pos_x + sub_w] = sub_frame.data

        # generate subtexture meta information object
        hotspot_x, hotspot_y = sub_frame.hotspot
        drawn_frames_meta.append(subtexture_meta(pos_x, pos_y,
                                                 sub_w, sub_h,
                                                 hotspot_x, hotspot_y))

    atlas = TextureImage(atlas_data)

    spam("successfully merged %d frames to atlas." % len(frames))

    return atlas, (width, height), drawn_frames_meta
