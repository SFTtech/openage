#!/usr/bin/env python3

# Copyright 2018-2018 the openage authors. See copying.md for legal info.

"""
Transforms the dimetric tiles converted from SLP files to cartesian format.
"""

import math
import argparse
from .slp import SLP
from .texture import Texture
from PIL import Image
import numpy as np

class Terrain_convert:
    
    # Method to simply put the tile on the big merge img. Something is wrong on this method because the result is transparent.
    # TO FIX.
    @staticmethod
    def replace(overlayer, overlayed, overlayer_x, overlayer_y, overlayed_x, overlayed_y, slp_width, slp_height):
        to_ret = overlayed
        xr = overlayer_x
        xd = overlayed_x
        yr = overlayer_y
        yd = overlayed_y
        while xd < overlayed.shape[0]:
            while yd < overlayed.shape[1]:
                # if the overlayer is not transparent, then we can push it on the overlayed
                # without the if, we would overwrite non-transparent pixels with transparent ones.
                if xr < overlayer.shape[0] and yr < overlayer.shape[1] and overlayer[xr,yr][3]>0:
                    to_ret[xd,yd] = overlayer[xr,yr]
                yd+=1
                yr+=1
            xd+=1
            xr+=1
        return to_ret

    
    #Result of this method is transparent.
    # TO FIX.
    @staticmethod
    def merge(img):
        """
        This operation merges the single tiles together. The image converted
        from SLP contains 100 (10x10) different tiles for each terrain.
        """

        # Our merge result will be slightly smaller than the SLP image
        merge_img = np.empty([962,481,4], dtype=np.ndarray)
        merge_img.fill(0)
        transposed_img = img
        slp_tile_width = 98
        slp_tile_height = 49
        # Merge offset
        merge_offset_x = (0.5) * slp_tile_width
        merge_offset_y = math.floor((0.5) * slp_tile_height)
        
        slp_y = 0
        while slp_y <  10:
            
            # The (x,y) coordinates of the tile from the source image
            slp_coord_x = 0
            slp_coord_y = slp_y * (slp_tile_height + 1)
            merge_coord_y = 216 - (slp_y * merge_offset_y)
            for slp_x in range(0, 10):
                
                # The destination x-coordinate in the merge image
                merge_coord_x = (int)((slp_x * (merge_offset_x -1))
                                    + slp_y * (merge_offset_x - 1))
                # method that replaces alpha_composite. Is it the good way ?
                merge_img = Terrain_convert.replace(transposed_img, merge_img, slp_coord_x, slp_coord_y, merge_coord_x, merge_coord_y, slp_tile_width, slp_tile_height)

                merge_coord_y = merge_coord_y+  merge_offset_y
            slp_y+=1
        return merge_img
    
    # Not yet verified. One step at the time :)
    @staticmethod
    def transform(img):
        """
        Dimetric to flat transformation.
        """
        res_x, res_y = img.shape[0], img.shape[1]

        tr_res_x = (int)((0.5) * res_x)

        #tr_img = Image.new('RGBA', (tr_res_x, res_y), (0, 0, 0, 0))
        tr_img = np.empty([tr_res_x,res_y,4], dtype=np.ndarray)
        tr_img.fill(0)
        # Get the pixels
        org_pixels = img
        tr_pixels = tr_img

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

        #tr_img = tr_img.rotate(270)
        return tr_img
    
    
