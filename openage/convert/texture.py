# Copyright 2014-2016 the openage authors. See copying.md for legal info.

""" Routines for texture generation etc """

# TODO pylint: disable=C,R

import math


from .blendomatic import BlendingMode
from .dataformat import (exportable, data_definition,
                         struct_definition, data_formatter)
from .hardcoded.terrain_tile_size import TILE_HALFSIZE
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

    def __init__(self, input_data, palette=None):
        super().__init__()
        spam("creating Texture from %s" % (repr(input_data)))

        from .slp import SLP

        if isinstance(input_data, SLP):
            if palette is None:
                raise Exception("palette needed for SLP -> texture generation")
            frames = [
                # TODO this needs some _serious_ performance work
                # (at least a 10x improvement, 50x would be better).
                # ideas: remove PIL and use libpng via CPPInterface,
                #        cythonize parts of SLP.py
                TextureImage(frame.get_picture_data(palette, self.player_id),
                             hotspot=frame.info.hotspot)
                for frame in input_data.frames
            ]
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


def merge_frames(frames, max_width=0, max_height=0):
    """
    merge all given frames of this slp to a single image file.

    frames = [TextureImage, ...]

    returns = TextureImage, [drawn_frames_meta]
    """

    import numpy

    # TODO: actually optimize free space on the texture.
    # if you ever wanted to implement a combinatoric optimisation
    # algorithm, go for it, this function just waits for you.
    # https://en.wikipedia.org/wiki/Bin_packing_problem
    #
    # for now, using max values for solving bin packing problem
    # after determining the maximum frame width and height,
    # each frame will be placed in a grid with fields of these sizes.
    # this wastes storage, but works. If your blood boils now because you
    # wanna optimize for the best alignment, read the above notice again,
    # and implement a better version.

    if len(frames) == 0:
        raise Exception("cannot create texture with empty input frame list")

    # single-frame texture, no merging needed
    elif len(frames) == 1:
        cx, cy = frames[0].hotspot
        w, h = frames[0].width, frames[0].height
        return frames[0], (w, h), [subtexture_meta(0, 0, w, h, cx, cy)]

    # if not predefined, get maximum frame size by checking all frames
    if max_width == 0 or max_height == 0:
        max_width = max([teximg.width for teximg in frames])
        max_height = max([teximg.height for teximg in frames])

    max_per_row = math.ceil(math.sqrt(len(frames)))
    num_rows = math.ceil(len(frames) / max_per_row)

    # we leave 1 pixel free in between two sprites
    free_space_px = 1
    width = (max_width + free_space_px) * max_per_row
    height = (max_height + free_space_px + 1) * num_rows

    spam("merging %d frames to %dx%d atlas, %d pics per row, %d rows." % (
        len(frames), width, height, max_per_row, num_rows))

    # resulting draw pane
    atlas_data = numpy.zeros((height, width, 4), dtype=numpy.uint8)
    pos_x = 0
    pos_y = 0

    drawn_frames_meta = list()
    drawn_current_row = 0

    for sub_frame in frames:
        sub_w = sub_frame.width
        sub_h = sub_frame.height

        spam("drawing frame %03d on atlas at %d x %d..." % (
            len(drawn_frames_meta), pos_x, pos_y))

        atlas_data[pos_y:pos_y + sub_h, pos_x:pos_x + sub_w] = sub_frame.data

        # generate subtexture meta information object
        hotspot_x, hotspot_y = sub_frame.hotspot
        drawn_frames_meta.append(subtexture_meta(pos_x, pos_y,
                                                 sub_w, sub_h,
                                                 hotspot_x, hotspot_y))

        drawn_current_row += 1

        # place the subtexture with a 1px border
        pos_x += max_width + free_space_px

        # see if we have to start a new row now
        if drawn_current_row > max_per_row - 1:
            drawn_current_row = 0
            pos_x = 0
            pos_y += max_height + free_space_px

    atlas = TextureImage(atlas_data)

    spam("successfully merged %d frames to atlas." % len(frames))

    return atlas, (width, height), drawn_frames_meta
