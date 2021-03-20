# Copyright 2021-2021 the openage authors. See copying.md for legal info.
#
# cython: infer_types=True
# pylint: disable=too-many-locals
"""
Merges a texture containing several terrain tiles into a single cartesian
terrain texture.
"""

import numpy

from openage.convert.entity_object.export.texture import TextureImage

cimport cython
cimport numpy

from libc.math cimport sqrt, floor, ceil


def merge_terrain(texture):
    """
    Python wrapper for the Cython function.

    :param texture: Texture containing the terrain tiles.
    :type texture: Texture
    """
    cmerge_terrain(texture)


@cython.boundscheck(False)
@cython.wraparound(False)
@cython.cdivision(True)
cdef void cmerge_terrain(texture):
    """
    Merges tiles from an AoC terrain SLP into a single flat texture.

    You can imagine every terrain tile (frame) as a puzzle piece
    of the big merged terrain. By blending and overlapping
    the tiles, we create a single terrain texture.

    :param texture: Texture containing the terrain tiles.
    :type texture: Texture
    """
    frames = texture.frames
    # Can be 10 (regular terrain) or 6 (farms)
    cdef unsigned int tiles_per_row = <int>sqrt(len(frames))

    # Size of one tile should be (98,49)
    cdef unsigned int frame_width = frames[0].width
    cdef unsigned int frame_height = frames[0].height

    cdef unsigned int flat_frame_width = (frame_width // 2) + 1
    cdef unsigned int flat_frame_height = frame_height

    cdef numpy.uint8_t[:, :, ::1] csubframe_atlas

    cdef unsigned int column_idx
    cdef unsigned int row_idx

    cdef unsigned int merge_atlas_width = (frame_width * tiles_per_row) - (tiles_per_row - 1)
    cdef unsigned int merge_atlas_height = (frame_height * tiles_per_row) - (tiles_per_row - 1)
    cdef unsigned int flat_atlas_width = (merge_atlas_width // 2) + 1

    cdef numpy.ndarray[numpy.uint8_t, ndim=3, mode="c"] flat_atlas = \
        numpy.zeros((merge_atlas_height, flat_atlas_width, 4), dtype=numpy.uint8)
    cdef numpy.uint8_t[:, :, ::1] cflat_atlas = flat_atlas

    cdef unsigned int source_x
    cdef unsigned int source_y

    cdef unsigned int final_x
    cdef unsigned int final_y

    index = 0
    for sub_frame in frames:
        csubframe_atlas = sub_frame.data

        # Fill each column upwards, starting with the last row
        row_idx = (tiles_per_row - 1) - (index % tiles_per_row)
        column_idx = index // tiles_per_row

        # Does a matrix transformation using
        # [  1 , -1  ]
        # [ 0.5, 0.5 ]
        # as the multipication matrix.
        # This reverses the dimetric projection (diamond shape view)
        # to a plan projection (bird's eye view).
        # Reference: https://gamedev.stackexchange.com/questions/
        #            16746/what-is-the-name-of-perspective-of-age-of-empires-ii
        for target_x in range(flat_frame_width):
            for target_y in range(frame_height):
                # Find the coords of the pixel (source) that is projected
                # to the target pixel coords
                source_x = (target_x + flat_frame_width - 1) - target_y
                source_y = <unsigned int>floor(0.5 * target_x + 0.5 * target_y)

                if target_x + target_y < frame_height:
                    source_y = int(ceil(0.5 * target_x + 0.5 * target_y))

                # Offset the target coords for the big texture
                final_x = target_x + (column_idx * (flat_frame_width - 1))
                final_y = target_y + (row_idx * (flat_frame_height - 1))

                cflat_atlas[final_y, final_x] = csubframe_atlas[source_y, source_x]

        index += 1

    # Rotate by 270 degrees to match the rotation of HD terrain textures
    flat_atlas = numpy.ascontiguousarray(numpy.rot90(flat_atlas, 3, axes=(0, 1)))

    texture.image_data = TextureImage(flat_atlas)
