# Copyright 2020-2020 the openage authors. See copying.md for legal info.

from . cimport libpng
from libc.stdint cimport uint8_t
from libcpp.vector cimport vector

cdef extern from *:
    """
    // Copyright 2021-2021 the openage authors. See copying.md for legal info.

    #include "png.h"

    #include <cstdint>
    #include <cstdlib>
    #include <cstring>

    struct tmp_file_buffer_state {
        png_bytep buffer;
        size_t size;
    };

    void tmp_file_png_write_fn(png_structp png_ptr, png_bytep data, png_size_t length) {
        // Get buffer ptr from libpng
        tmp_file_buffer_state *state = (tmp_file_buffer_state *)png_get_io_ptr(png_ptr);
        size_t new_size = state->size + length;

        // Resize buffer to fit new data
        if (state->buffer) {
            state->buffer = (png_bytep)realloc(state->buffer, new_size);
        }
        else {
            state->buffer = (png_bytep)malloc(new_size);
        }

        if (!state->buffer) {
            png_error(png_ptr, "Error allocating memory for temporary in-memory file.");
        }

        // Append new data to buffer
        memcpy(state->buffer + state->size, data, length);
        state->size = new_size;
    }

    void tmp_file_flush_fn(png_structp png_ptr) {
        // Do nothing, since changes don't need to be written to disk
    }
    """
    ctypedef struct tmp_file_buffer_state:
        libpng.png_bytep buffer
        size_t size

    void tmp_file_png_write_fn(libpng.png_structp png_ptr,
                               libpng.png_bytep data,
                               libpng.png_size_t)
    void tmp_file_flush_fn(libpng.png_structp png_ptr)
