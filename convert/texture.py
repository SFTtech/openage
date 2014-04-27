#!/usr/bin/env python3
#
#routines for texture generation etc

from blendomatic import BlendingMode
import dataformat
import math
import numpy
from slp import SLP
import util
from util import dbg


def subtexture_meta(tx, ty, hx, hy, cx, cy):
    """
    generate a dict that contains the meta information for
    the given parameters:
        origin x, y
        heigh, width
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

        self.width  = picture_data.shape[1]
        self.height = picture_data.shape[0]

        if hotspot is None:
            self.hotspot = (0, 0)
        else:
            self.hotspot = hotspot

        self.data = picture_data

        dbg("created TextureImage with size %dx%d" % (self.width, self.height), 3)

    def get_pil_image(self):
        from PIL import Image
        return Image.fromarray(self.data)


class Texture:
    image_format       = "png"

    name_struct        = "subtexture"
    name_struct_file   = "texture"
    struct_description = """one sprite, as part of a texture atlas.

this struct stores information about what position and size
one sprite included in the 'big texture' has."""
    data_format = (
        ("x",  "int32_t"),
        ("y",  "int32_t"),
        ("w",  "int32_t"),
        ("h",  "int32_t"),
        ("cx", "int32_t"),
        ("cy", "int32_t"),
    )

    #player-specific colors will be in color blue, but with an alpha of 254
    player_id = 1

    def __init__(self, input_data, palette=None):

        dbg("creating Texture from %s" % (repr(input_data)), 3)

        if isinstance(input_data, SLP):

            if palette is None:
                raise Exception("creating a texture from a SLP requires a palette")
            frames = [
                TextureImage(
                    frame.get_picture_data(palette, self.player_id),
                    hotspot=frame.info.hotspot,
                )
                for frame in input_data.frames
            ]
        elif isinstance(input_data, BlendingMode):
            frames = [
                TextureImage(
                    tile.get_picture_data(),
                )
                for tile in input_data.alphamasks
            ]
        else:
            raise Exception("cannot create Texture from unknown source type")

        self.image_data, (self.width, self.height), self.image_metadata = merge_frames(frames)

    def save(self, filename, meta_formats):
        """
        save the texture to the given filename
        """

        #store the image data as png
        raw_png = util.VirtualFile()
        image = self.image_data.get_pil_image()
        image.save(raw_png, self.image_format)

        #generate formatted texture metadata
        formatter = dataformat.DataFormatter()
        formatter.add_data(self.dump(filename))

        #generate full output file contents
        output_data = formatter.export(meta_formats)
        output_data["%s.%s" % (filename, self.image_format)] = raw_png.data()

        #save the output files
        util.file_write_multi(output_data)

    def dump(self, filename):
        return [ dataformat.DataDefinition(self, self.image_metadata, filename) ]

    @classmethod
    def structs(cls):
        return [ dataformat.StructDefinition(cls) ]


def merge_frames(frames, max_width=0, max_height=0):
    """
    merge all given frames of this slp to a single image file.

    frames = [TextureImage, ...]

    returns = TextureImage, [drawn_frames_meta]
    """

    #TODO: actually optimize free space on the texture.
    #if you ever wanted to implement a combinatoric optimisation
    #algorithm, go for it, this function just waits for you.
    #https://en.wikipedia.org/wiki/Bin_packing_problem

    #for now, using max values for solving bin packing problem
    #after determining the maximum frame width and height,
    #each frame will be placed in a grid with fields of these sizes.
    #this wastes storage, but works. If your blood boils now because you
    #wanna optimize for the best alignment, read the above notice again,
    #and implement a better version.

    if len(frames) == 0:
        raise Exception("cannot create texture with empty input frame list")

    #single-frame texture, no merging needed
    elif len(frames) == 1:
        cx, cy = frames[0].hotspot
        w,  h  = frames[0].width, frames[0].height
        return frames[0], (w, h), [subtexture_meta(0, 0, w, h, cx, cy)]

    #if not predefined, get maximum frame size by checking all frames
    if max_width == 0 or max_height == 0:
        max_width  = max([teximg.width  for teximg in frames])
        max_height = max([teximg.height for teximg in frames])

    max_per_row = math.ceil(math.sqrt(len(frames)))
    num_rows    = math.ceil(len(frames) / max_per_row)

    #we leave 1 pixel free in between two sprites
    free_space_px = 1
    width  = (max_width  + free_space_px)     * max_per_row
    height = (max_height + free_space_px + 1) * num_rows

    dbg("merging %d frames to %dx%d atlas, %d pics per row, %d rows." % (len(frames), width, height, max_per_row, num_rows), 2)

    #resulting draw pane
    draw_data = list()
    for _ in range(height):
        row_data = list()
        for _ in range(width):
            row_data.append((0, 0, 0, 0))
        draw_data.append(row_data)

    pos_x = 0
    pos_y = 0

    drawn_frames_meta = list()
    drawn_current_row = 0

    for sub_frame in frames:
        sub_w = sub_frame.width
        sub_h = sub_frame.height

        dbg("drawing frame %03d on atlas at % 3dx% 3d..." % (len(drawn_frames_meta), pos_x, pos_y), 3)

        for y, row_data in enumerate(sub_frame.data):
            for x, pixel_data in enumerate(row_data):
                draw_data[y + pos_y][x + pos_x] = pixel_data

        #generate subtexture meta information object
        hotspot_x, hotspot_y = sub_frame.hotspot
        drawn_frames_meta.append(subtexture_meta(pos_x, pos_y, sub_w, sub_h, hotspot_x, hotspot_y))

        drawn_current_row += 1

        #place the subtexture with a 1px border
        pos_x += max_width + free_space_px

        #see if we have to start a new row now
        if drawn_current_row > max_per_row - 1:
            drawn_current_row = 0
            pos_x = 0
            pos_y += max_height + free_space_px

    atlas_data = numpy.array(draw_data, dtype=numpy.uint8)
    atlas = TextureImage(atlas_data)

    return atlas, (width, height), drawn_frames_meta
