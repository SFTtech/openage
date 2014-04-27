#!/usr/bin/env python3
#
#routines for texture generation etc

from blendomatic import BlendingMode
import dataformat
import math
from png import PNG
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

        if isinstance(input_data, SLP):
            frames = [
                (
                    PNG(frame.get_picture_data(), self.player_id, palette),
                    frame.info.hotspot,
                )
                for frame in input_data.frames
            ]
        elif isinstance(input_data, BlendingMode):
            frames = [
                (
                    PNG(tile["data"], w=tile["width"], h=tile["height"], alphamask=True),
                    (0, 0),
                )
                for tile in input_data.alphamasks
            ]
        else:
            raise Exception("cannot create Texture from unknown source type")

        #self.frames now is a list of frames.
        #   frames:      hotspot:
        #[ (frame=PNG, (cx, cy)) ]

        self.image, self.image_metadata = merge_frames(frames)
        self.raw_png = util.VirtualFile()

    def save(self, filename, meta_formats):
        """
        save the texture to the given filename
        """

        #store the image data as png
        self.image.save(self.raw_png, self.image_format)

        #generate formatted texture metadata
        formatter = dataformat.DataFormatter()
        formatter.add_data(self.dump(filename))

        #generate full output file contents
        output_data = formatter.export(meta_formats)
        output_data["%s.%s" % (filename, self.image_format)] = self.raw_png.data()

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

    frames = [(PNG, (hotspot_x, hotspot_y)), ... ]

    returns = atlas, [drawn_frames_meta]
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

    from PIL import Image

    if len(frames) == 0:
        raise Exception("cannot create texture with empty input frame list")

    elif len(frames) == 1:
        png, (cx, cy) = frames[0]
        image = png.image
        w, h  = image.size

        single_texture_meta = subtexture_meta(0, 0, w, h, cx, cy)

        #return the single-frame texture
        return image, [single_texture_meta]


    #if not predefined, get maximum frame size by checking all frames
    if max_width == 0 or max_height == 0:
        for (png, _) in frames:
            w, h = png.image.size
            if w > max_width:
                max_width = w
            if h > max_height:
                max_height = h


    max_per_row = math.ceil(math.sqrt(len(frames)))
    num_rows    = math.ceil(len(frames) / max_per_row)

    #we leave 1 pixel free in between two sprites
    free_space_px = 1
    width  = math.ceil((max_width  + free_space_px)     * max_per_row)
    height = math.ceil((max_height + free_space_px + 1) * num_rows)

    dbg("merging %d frames to %dx%d atlas, %d pics per row, %d rows." % (len(frames), width, height, max_per_row, num_rows), 2)

    #create the big atlas image where the small ones will be placed on
    atlas = Image.new('RGBA', (width, height), (0, 0, 0, 0))

    pos_x = 0
    pos_y = 0

    drawn_frames_meta = list()
    drawn_current_row = 0

    for sub_frame, (hotspot_x, hotspot_y) in frames:
        subtexture = sub_frame.image
        sub_w = subtexture.size[0]
        sub_h = subtexture.size[1]
        box   = (pos_x, pos_y, pos_x + sub_w, pos_y + sub_h)

        atlas.paste(subtexture, box) #, mask=subtexture)
        dbg("drew frame %03d on atlas at %dx%d " % (len(drawn_frames_meta), pos_x, pos_y), 3)

        #generate subtexture meta information dict
        drawn_subtexture_meta = subtexture_meta(pos_x, pos_y, sub_w, sub_h, hotspot_x, hotspot_y)
        drawn_frames_meta.append(drawn_subtexture_meta)

        drawn_current_row = drawn_current_row + 1

        #place the subtexture with a 1px border
        pos_x = pos_x + max_width + 1

        #see if we have to start a new row now
        if drawn_current_row > max_per_row - 1:
            drawn_current_row = 0
            pos_x = 0
            pos_y = pos_y + max_height + 1

    return atlas, drawn_frames_meta
