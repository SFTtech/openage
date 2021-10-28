# Copyright 2020-2020 the openage authors. See copying.md for legal info.

from . cimport libpng
from libc.stdint cimport uint8_t

cdef extern from "png_tmp_file.h":
    ctypedef struct tmp_file_buffer_state:
        char* buffer
        size_t size

    void tmp_file_png_write_fn(libpng.png_structp png_ptr,
                               libpng.png_bytep data,
                               libpng.png_size_t)
    void tmp_file_flush_fn(libpng.png_structp png_ptr)
    int check_compressed_size(uint8_t ***imagedata,
                              int compression_level,
                              int memory_level,
                              int compression_strategy,
                              int filters,
                              int width,
                              int height)
