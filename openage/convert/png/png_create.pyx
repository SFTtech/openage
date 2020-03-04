# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# cython: profile=False

from libc.stdio cimport fopen, fclose
from libc.stdint cimport uint8_t
from libc.stdlib cimport realloc
from cpython.mem cimport PyMem_Malloc

from . cimport libpng

cimport cython
import numpy
cimport numpy


@cython.boundscheck(False)
@cython.wraparound(False)
def save(filename, numpy.ndarray[numpy.uint8_t, ndim=3, mode="c"] imagedata not None):
    """
    Save an image as a PNG file.
    """
    cdef unsigned int width = imagedata.shape[1]
    cdef unsigned int height = imagedata.shape[0]
    cdef numpy.uint8_t[:,:,::1] mview = imagedata
    png_create(filename.encode('UTF-8'), mview, width, height)


@cython.boundscheck(False)
@cython.wraparound(False)
cdef void png_create(char* filename, numpy.uint8_t[:,:,::1] imagedata,
                     int width, int height):
    """
    Create a PNG file with libpng and write it to file.
    """
    cdef libpng.png_FILE_p fp = fopen(filename, "wb")

    cdef libpng.png_structp png
    cdef libpng.png_infop info

    png = libpng.png_create_write_struct(libpng.PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)
    info = libpng.png_create_info_struct(png)
    
    libpng.png_init_io(png, fp)
    libpng.png_set_IHDR(png, 
                        info,
                        width,
                        height,
                        8,
                        libpng.PNG_COLOR_TYPE_RGBA,
                        libpng.PNG_INTERLACE_NONE,
                        libpng.PNG_COMPRESSION_TYPE_DEFAULT,
                        libpng.PNG_FILTER_TYPE_DEFAULT)
    libpng.png_write_info(png, info)
    
    for row_idx in range(height):
        libpng.png_write_row(png, &imagedata[row_idx,0,0])

    libpng.png_write_end(png, info)

    # TODO: This doesn't work, but would be faster
    # libpng.png_set_rows(png, info, imagedata)
    # libpng.png_write_png(png, info, libpng.PNG_TRANSFORM_IDENTITY, NULL)
    
    fclose(fp)
    
    libpng.png_destroy_write_struct(&png, &info)