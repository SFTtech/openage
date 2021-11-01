# Copyright 2020-2020 the openage authors. See copying.md for legal info.

from . cimport libpng
from libc.stdint cimport uint8_t

cdef extern from *:
    """
    // Copyright 2021-2021 the openage authors. See copying.md for legal info.

    #include "png.h"

    #include <cstdint>
    #include <cstdlib>
    #include <cstring>

    struct tmp_file_buffer_state {
        char *buffer;
        size_t size;
    };

    void tmp_file_png_write_fn(png_structp png_ptr, png_bytep data, png_size_t length) {
        // Get buffer ptr from libpng
        tmp_file_buffer_state *state = (tmp_file_buffer_state *)png_get_io_ptr(png_ptr);
        size_t new_size = state->size + length;

        // Resize buffer to fit new data
        if (state->buffer) {
            state->buffer = (char *)realloc(state->buffer, new_size);
        }
        else {
            state->buffer = (char *)malloc(new_size);
        }

        if (!state->buffer) {
            png_error(png_ptr, "Error allocating memory for temporary in-memory file.");
        }

        // Append new data to buffer
        memcpy(state->buffer + state->size, data, length);
    }

    void tmp_file_flush_fn(png_structp png_ptr) {
        // Do nothing, since changes don't need to be written to disk
    }

    int check_compressed_size(uint8_t ***imagedata, int compression_level, int memory_level, int compression_strategy, int filters, int width, int height) {
        png_structp write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        png_infop write_info_ptr = png_create_info_struct(write_ptr);

        // Configure write settings
        png_set_compression_level(write_ptr, compression_level);
        png_set_compression_mem_level(write_ptr, memory_level);
        png_set_compression_strategy(write_ptr, compression_strategy);
        png_set_filter(write_ptr, PNG_FILTER_TYPE_DEFAULT, filters);
        png_set_IHDR(write_ptr, write_info_ptr, width, height, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

        // Initialize temporary 'file' in memory to write to
        tmp_file_buffer_state state;
        state.buffer = NULL;
        state.size = 0;

        // Set libpng to write to in-memory temporary file
        png_set_write_fn(write_ptr, &state, tmp_file_png_write_fn, tmp_file_flush_fn);

        // Write data
        png_write_info(write_ptr, write_info_ptr);

        for (int row_idx = 0; row_idx < height; row_idx++) {
            png_write_row(write_ptr, imagedata[row_idx][0]);
        }

        // Cleanup
        png_write_end(write_ptr, write_info_ptr);
        png_destroy_write_struct(&write_ptr, &write_info_ptr);

        if (state.buffer) {
            free(state.buffer);
        }

        return state.size;
    }
    """
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
