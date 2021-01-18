# Copyright 2014-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals
"""
Merges texture frames into a spritesheet or terrain tiles into
a terrain texture.
"""
import numpy

from ....log import spam
from ...entity_object.export.texture import TextureImage
from ...service.export.png.binpack import DeterministicPacker
from ...service.export.png.binpack import RowPacker, ColumnPacker, BinaryTreePacker, BestPacker
from ...value_object.read.media.hardcoded.texture import (MAX_TEXTURE_DIMENSION, MARGIN,
                                                          TERRAIN_ASPECT_RATIO)


def merge_frames(texture, custom_packer=None, replay=None):
    """
    merge all given frames in a texture into a single image atlas.

    frames = [TextureImage, ...]

    returns = TextureImage, (width, height), [drawn_frames_meta]
    """
    frames = texture.frames
    if len(frames) == 0:
        raise Exception("cannot create texture with empty input frame list")

    if custom_packer:
        packer = custom_packer

    elif replay:
        packer = DeterministicPacker(
            margin=MARGIN,
            hints=replay
        )

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

    texture.image_data = TextureImage(atlas_data)
    texture.image_metadata = drawn_frames_meta

    spam("successfully merged %d frames to atlas.", len(frames))

    if isinstance(packer, BestPacker):
        # Only generate these values if no custom packer was used
        # TODO: It might make sense to do it anyway for debugging purposes
        texture.best_packer_hints = packer.get_mapping_hints(frames)
