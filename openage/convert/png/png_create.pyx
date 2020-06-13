# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# cython: profile=False

from libc.stdio cimport _IOFBF, SEEK_END, SEEK_SET, fprintf, fclose, fread, fseek, ftell, rewind, setvbuf, tmpfile
from libc.stdint cimport uint8_t
from libc.stdlib cimport malloc, free
from libc.string cimport memcpy, memset

from posix.stdio cimport open_memstream

from ..opus.bytearray cimport PyByteArray_AS_STRING
from . cimport libpng
from enum import Enum

cimport cython
import numpy
cimport numpy


class CompressionMethod(Enum):
    COMPR_NONE       = 0x00  # unused; no compression (for debugging)
    COMPR_DEFAULT    = 0x01  # no optimization; default PNG compression
    COMPR_GREEDY     = 0x02  # try several compression parameters; usually 50% smaller files
    COMPR_AGGRESSIVE = 0x04  # unused; use zopfli for even better compression

cdef struct greedy_replay_param:
    uint8_t compr_lvl
    uint8_t mem_lvl
    uint8_t strat
    uint8_t filters

cdef struct process:
    int     best_filesize
    uint8_t best_compr_lvl
    uint8_t best_compr_mem_lvl
    uint8_t best_compr_strat
    uint8_t best_filters

# Running OptiPNG with optimization level 2 (-o2 flag)
cdef int GREEDY_COMPR_LVL_MIN = 9
cdef int GREEDY_COMPR_LVL_MAX = 9
cdef int GREEDY_COMPR_MEM_LVL_MIN = 8
cdef int GREEDY_COMPR_MEM_LVL_MAX = 8
cdef int GREEDY_COMPR_STRAT_MIN = 0
cdef int GREEDY_COMPR_STRAT_MAX = 3
cdef int GREEDY_FILTER_0 = libpng.PNG_FILTER_NONE
cdef int GREEDY_FILTER_5 = libpng.PNG_ALL_FILTERS


@cython.boundscheck(False)
@cython.wraparound(False)
def save(numpy.ndarray[numpy.uint8_t, ndim=3, mode="c"] imagedata not None,
         compr_method=CompressionMethod.COMPR_DEFAULT, compr_settings=None):
    """
    Save an image as a PNG file.
    
    :param imagedata: A 3-dimensional array with RGBA color values for pixels.
    :type imagedata: numpy.ndarray
    :param compr_method: The compression optimization method.
    :type compr_method: CompressionMethod
    :param compr_settings: A 4-tuple that containing compression level,
                           memory level, strategy and filter method (in that
                           order) used for encoding the PNG.
    :type compr_settings: tuple
    """
    cdef unsigned int width = imagedata.shape[1]
    cdef unsigned int height = imagedata.shape[0]
    cdef numpy.uint8_t[:,:,::1] mview = imagedata

    cdef greedy_replay_param replay

    if compr_method is CompressionMethod.COMPR_GREEDY:
        if compr_settings:
            replay.compr_lvl = compr_settings[0]
            replay.mem_lvl = compr_settings[1]
            replay.strat = compr_settings[2]
            replay.filters = compr_settings[3]

        else:
            replay.compr_lvl = 0xFF
            replay.mem_lvl = 0xFF
            replay.strat = 0xFF
            replay.filters = 0xFF

        outdata, best_settings = optimize_greedy(mview, width, height, replay)

    else:
        outdata = optimize_default(mview, width, height)
        best_settings = None

    return outdata, best_settings


@cython.boundscheck(False)
@cython.wraparound(False)
cdef bytearray optimize_default(numpy.uint8_t[:,:,::1] imagedata, int width, int height):
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

@cython.boundscheck(False)
@cython.wraparound(False)
cdef optimize_greedy(numpy.uint8_t[:,:,::1] imagedata, int width, int height, greedy_replay_param replay):
    """
    Reimplementation of OptiPNG.
    """
    if replay.compr_lvl == 0xFF:
        replay = optimize_greedy_iterate(imagedata, width, height)

    # Create an in-memory stream of a file
    cdef char *outbuffer
    cdef size_t outbuffer_len
    cdef libpng.png_FILE_p fp = open_memstream(&outbuffer, &outbuffer_len)

    write_to_file(imagedata, fp,
                  replay.compr_lvl,
                  replay.mem_lvl,
                  replay.strat,
                  replay.filters,
                  width, height)

    # Copy file data to bytearray
    fseek(fp, 0, SEEK_END)
    filesize = ftell(fp)
    rewind(fp)

    outdata = bytearray(filesize)
    cdef char *out = PyByteArray_AS_STRING(outdata)
    wresult = fread(out, 1, filesize, fp)

    if wresult != filesize:
        raise MemoryError("Copy to bytearray failed for PNG conversion.")

    # Free memory
    fclose(fp)
    free(outbuffer)

    return outdata, replay


@cython.boundscheck(False)
@cython.wraparound(False)
cdef greedy_replay_param optimize_greedy_iterate(numpy.uint8_t[:,:,::1] imagedata, int width, int height):
    """
    Reimplementation of OptiPNG.
    """
    cdef int     best_filesize = 0x7fffffff
    cdef int     current_filesize = 0x7fffffff
    cdef uint8_t best_compr_lvl = 0xFF
    cdef uint8_t best_compr_mem_lvl = 0xFF
    cdef uint8_t best_compr_strat = 0xFF
    cdef uint8_t best_filters = 0xFF

    cdef greedy_replay_param result

    # Create an in-memory stream of a file
    cdef char *buf
    cdef size_t len
    cdef libpng.png_FILE_p fp

    for filters in range(GREEDY_FILTER_0, GREEDY_FILTER_5 + 1):
        if filters != GREEDY_FILTER_0 and filters != GREEDY_FILTER_5:
            continue

        for strategy in range(GREEDY_COMPR_STRAT_MIN, GREEDY_COMPR_STRAT_MAX + 1):
            for compr_lvl in range(GREEDY_COMPR_LVL_MIN, GREEDY_COMPR_LVL_MAX + 1):
                for mem_lvl in range(GREEDY_COMPR_MEM_LVL_MIN, GREEDY_COMPR_MEM_LVL_MAX + 1):
                    fp = open_memstream(&buf, &len)

                    # Write the file
                    write_to_file(imagedata, fp, compr_lvl, mem_lvl,
                                   strategy, filters, width, height)

                    # Check how big the result is
                    fseek(fp, 0, SEEK_END)
                    current_filesize = ftell(fp)

                    if current_filesize < best_filesize:
                        best_compr_lvl = compr_lvl
                        best_compr_mem_lvl = mem_lvl
                        best_compr_strat = strategy
                        best_filters = filters
                        best_filesize = current_filesize

                    fclose(fp)

    free(buf)

    result.compr_lvl = best_compr_lvl
    result.mem_lvl = best_compr_mem_lvl
    result.strat = best_compr_strat
    result.filters = best_filters

    return result


@cython.boundscheck(False)
@cython.wraparound(False)
cdef void write_to_file(numpy.uint8_t[:,:,::1] imagedata,
                          libpng.png_FILE_p fp,
                          int compression_level, int memory_level,
                          int compression_strategy, int filters,
                          int width, int height):
    """
    Write the PNG to a file.
    """
    write_ptr = libpng.png_create_write_struct(libpng.PNG_LIBPNG_VER_STRING,
                                               NULL,
                                               NULL,
                                               NULL)
    write_info_ptr = libpng.png_create_info_struct(write_ptr)

    # Configure write settings
    libpng.png_set_compression_level(write_ptr, compression_level)
    libpng.png_set_compression_mem_level(write_ptr, memory_level)
    libpng.png_set_compression_strategy(write_ptr, compression_strategy)
    libpng.png_set_filter(write_ptr, libpng.PNG_FILTER_TYPE_DEFAULT, filters)

    libpng.png_init_io(write_ptr, fp)
    libpng.png_set_IHDR(write_ptr, write_info_ptr,
                        width, height,
                        8,
                        libpng.PNG_COLOR_TYPE_RGBA,
                        libpng.PNG_INTERLACE_NONE,
                        libpng.PNG_COMPRESSION_TYPE_DEFAULT,
                        libpng.PNG_FILTER_TYPE_DEFAULT)

    # Write the data
    libpng.png_write_info(write_ptr, write_info_ptr)

    for row_idx in range(height):
        libpng.png_write_row(write_ptr, &imagedata[row_idx,0,0])

    libpng.png_write_end(write_ptr, write_info_ptr)

    # Destroy the write struct    
    libpng.png_destroy_write_struct(&write_ptr, &write_info_ptr)
