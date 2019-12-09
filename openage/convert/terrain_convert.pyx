#!/usr/bin/env python3

# Copyright 2018-2018 the openage authors. See copying.md for legal info.

"""
Transforms the dimetric tiles converted from SLP files to cartesian format.
"""

import math
import argparse
from PIL import Image
from .slp import SLP
from .texture import Texture

class Terrain_convert:

    def merge(img):
        """
        This operation merges the single tiles together. The image converted
        from SLP contains 100 (10x10) different tiles for each terrain.
        """

        # Our merge result will be slightly smaller than the SLP image
        merge_img = Image.new('RGBA', (962, 481), (0, 0, 0, 0))

        # The size of each tile in the SLP
        cdef int slp_tile_width = 98
        cdef int slp_tile_height = 49
        # Merge offset
        merge_offset_x = (0.5) * slp_tile_width
        merge_offset_y = math.floor((0.5) * slp_tile_height)
        
        # The tiles in the SLP image work like a matrix. The tile at position
        # (0,0) will be put at the corresponding position - using AoE2 coordinates -
        # in the merge result. The AoE2 coordinate system origin point is in
        # the left corner, so our tile (0,0) goes there.
        cdef int slp_y = 0
        while slp_y <  10:

            # The (x,y) coordinates of the tile from the source image
            slp_coord_x = 0
            slp_coord_y = slp_y * (slp_tile_height + 1)
            merge_coord_y = 216 - (slp_y * merge_offset_y)
            for slp_x in range(0, 10):
                # The destination x-coordinate in the merge image
                merge_coord_x = (int)((slp_x * (merge_offset_x -1))
                                    + slp_y * (merge_offset_x - 1))
                merge_img.alpha_composite(img, (merge_coord_x, merge_coord_y),
                                        (slp_coord_x, slp_coord_y,
                                        (slp_coord_x + slp_tile_width),
                                        (slp_coord_y + slp_tile_height)))

                slp_coord_x = slp_coord_x + slp_tile_width
                merge_coord_y = merge_coord_y+  merge_offset_y
            slp_y+=1
        return merge_img

    def transform(img):
        """
        Dimetric to flat transformation.
        """
        res_x, res_y = img.size

        tr_res_x = (int)((0.5) * res_x)

        tr_img = Image.new('RGBA', (tr_res_x, res_y), (0, 0, 0, 0))

        # Get the pixels
        org_pixels = img.load()
        tr_pixels = tr_img.load()

        # Transform the image
        for x_coord in range(0, tr_res_x):
            for y_coord in range(0, res_y):

                # This uses the exact calculation as in transform()
                tr_x = (1 * x_coord + tr_res_x - 1) - 1 * y_coord
                if x_coord+y_coord < res_y:
                    tr_y = math.ceil(0.5 * x_coord + 0.5 * y_coord)
                else:
                    tr_y = math.floor(0.5 * x_coord + 0.5 * y_coord)

                # We just need to swap (tr_x,tr_y) and (x,y) from the other
                # function to revert the projection
                tr_pixels[x_coord, y_coord] = org_pixels[tr_x, tr_y]

        tr_img = tr_img.rotate(270)

        return tr_img