# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# cython: profile=False

from libc.stdint cimport uint8_t
from libc.stdlib cimport malloc, free
from libc.string cimport memcpy, memset

from ..opus.bytearray cimport PyByteArray_AS_STRING
from . cimport libpng

cimport cython
import numpy
cimport numpy


@cython.boundscheck(False)
@cython.wraparound(False)
def save(numpy.ndarray[numpy.uint8_t, ndim=3, mode="c"] imagedata not None):
    """
    Save an image as a PNG file.
    """
    cdef unsigned int width = imagedata.shape[1]
    cdef unsigned int height = imagedata.shape[0]
    cdef numpy.uint8_t[:,:,::1] mview = imagedata
    
    outdata = png_create(mview, width, height)
    
    return outdata


@cython.boundscheck(False)
@cython.wraparound(False)
cdef bytearray png_create(numpy.uint8_t[:,:,::1] imagedata, int width, int height):
    """
    Create a PNG file with libpng and write it to file.
    """
    # Define basic image data
    cdef libpng.png_image write_image
    memset(&write_image, 0, sizeof(write_image))
    write_image.version = libpng.PNG_IMAGE_VERSION
    write_image.width = width
    write_image.height = height
    write_image.format = libpng.PNG_FORMAT_RGBA

    # Get required byte size
    cdef libpng.png_alloc_size_t write_image_size = 0
    cdef void *rgb_data = &imagedata[0,0,0]
    cdef int wresult = libpng.png_image_write_to_memory(&write_image,
                                                        NULL,
                                                        &write_image_size,
                                                        0,
                                                        rgb_data,
                                                        0,
                                                        NULL)
    
    if not wresult:
        raise MemoryError("Could not allocate memory for PNG conversion.")

    # Write in buffer
    cdef void *outbuffer = malloc(write_image_size)
    wresult = libpng.png_image_write_to_memory(&write_image,
                                               outbuffer,
                                               &write_image_size,
                                               0,
                                               rgb_data,
                                               0,
                                               NULL)
    
    if not wresult:
        raise MemoryError("Write to buffer failed for PNG conversion.")
                          
    # Output data
    outdata = bytearray(write_image_size)
    cdef char *out = PyByteArray_AS_STRING(outdata)
    memcpy(out, outbuffer, write_image_size)
    free(outbuffer)

    return outdata
    