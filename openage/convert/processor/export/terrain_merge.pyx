# Copyright 2021-2021 the openage authors. See copying.md for legal info.
#
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
    """
    cmerge_terrain(texture)
    

@cython.boundscheck(False)
@cython.wraparound(False)
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
    cdef int tiles_per_row = int(sqrt(len(frames)))

    # Size of one tile should be (98,49)
    cdef int frame_width = frames[0].width
    cdef int frame_height = frames[0].height

    cdef int half_offset_x = frame_width // 2
    cdef int half_offset_y = frame_height // 2

    cdef int merge_atlas_width = (frame_width * tiles_per_row) - (tiles_per_row - 1)
    cdef int merge_atlas_height = (frame_height * tiles_per_row) - (tiles_per_row - 1)

    cdef numpy.ndarray[numpy.uint8_t, ndim=3, mode="c"] merge_atlas = \
        numpy.zeros((merge_atlas_height, merge_atlas_width, 4), dtype=numpy.uint8)
    
    cdef int merge_offset_x
    cdef int merge_offset_y

    cdef int merge_coord_x
    cdef int merge_coord_y

    cdef int tenth_frame
    cdef int every_frame

    for idx, sub_frame in enumerate(frames):
        tenth_frame = idx // tiles_per_row
        every_frame = idx % tiles_per_row

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


    # Transform to a flat texture
    cdef int flat_atlas_width = merge_atlas_width // 2 + 1

    cdef numpy.ndarray[numpy.uint8_t, ndim=3, mode="c"] flat_atlas = \
        numpy.zeros((merge_atlas_height, flat_atlas_width, 4), dtype=numpy.uint8)

    cdef int merge_x
    cdef int merge_y
    
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
            merge_y = int(floor(0.5 * flat_x + 0.5 * flat_y))

            if flat_x + flat_y < merge_atlas_height:
                merge_y = int(ceil(0.5 * flat_x + 0.5 * flat_y))

            flat_atlas[flat_y][flat_x] = merge_atlas[merge_y][merge_x]

    # Rotate by 270 degrees to match the rotation of HD terrain textures
    flat_atlas = numpy.ascontiguousarray(numpy.rot90(flat_atlas, 3, axes=(0, 1)))

    texture.image_data = TextureImage(flat_atlas)
