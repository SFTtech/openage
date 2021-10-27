# Copyright 2020-2020 the openage authors. See copying.md for legal info.

from libpng cimport png_size_t, png_structp, png_bytep

cdef extern from "png_tmp_file.h":
    cdef struct tmp_file_buffer_state:
        cdef char* buffer
        cdef size_t size
    cdef void tmp_file_png_write_fn(png_structp png_ptr, png_bytep data, png_size_t)
    cdef void tmp_file_flush_fn(png_structp png_ptr)
    cdef int check_compressed_size(uint8_t ***imagedata, int compression_level, int memory_level, int compression_strategy, int filters, int width, int height)
