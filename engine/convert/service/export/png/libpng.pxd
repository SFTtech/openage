# Copyright 2020-2023 the openage authors. See copying.md for legal info.

from libc.stdio cimport FILE

cdef extern from "png.h":
    const char PNG_LIBPNG_VER_STRING[]
    const int PNG_COLOR_TYPE_RGBA
    const int PNG_INTERLACE_NONE
    const int PNG_COMPRESSION_TYPE_DEFAULT
    const int PNG_FILTER_TYPE_DEFAULT
    const int PNG_TRANSFORM_IDENTITY
    const int PNG_IMAGE_VERSION
    const char PNG_FORMAT_RGBA

    const unsigned int PNG_FILTER_NONE
    const unsigned int PNG_ALL_FILTERS

    ctypedef unsigned char png_byte
    ctypedef const png_byte *png_const_bytep
    ctypedef png_byte *png_bytep
    ctypedef png_byte **png_bytepp
    ctypedef unsigned long int png_uint_32
    ctypedef long int png_int_32

    ctypedef struct png_struct
    ctypedef png_struct *png_structp
    ctypedef png_struct *png_structrp
    ctypedef png_struct **png_structpp
    ctypedef const png_struct *png_const_structrp

    ctypedef struct png_info
    ctypedef png_info *png_infop
    ctypedef png_info *png_inforp
    ctypedef png_info **png_infopp
    ctypedef const png_info *png_const_inforp

    ctypedef const char *png_const_charp
    ctypedef void *png_voidp
    ctypedef void *png_rw_ptr
    ctypedef void *png_flush_ptr
    ctypedef (png_structp, png_const_charp) *png_error_ptr
    ctypedef FILE *png_FILE_p

    ctypedef struct png_control
    ctypedef png_control *png_controlp
    ctypedef struct png_image:
        png_controlp opaque
        png_uint_32 version
        png_uint_32 width
        png_uint_32 height
        png_uint_32 format
        png_uint_32 flags
        png_uint_32 colormap_entries
        png_uint_32 warning_or_error
        char message[64]
    ctypedef png_image *png_imagep

    ctypedef size_t png_alloc_size_t

    png_structp png_create_write_struct(png_const_charp user_png_ver,
                                        png_voidp error_ptr,
                                        png_error_ptr error_fn,
                                        png_error_ptr warn_fn)
    png_infop png_create_info_struct(png_const_structrp png_ptr)

    void png_set_IHDR(png_const_structrp png_ptr,
                      png_inforp info_ptr,
                      png_uint_32 width,
                      png_uint_32 height,
                      int bit_depth,
                      int color_type,
                      int interlace_method,
                      int compression_method,
                      int filter_method)
    void png_init_io(png_structrp png_ptr,
                     png_FILE_p fp)
    void png_set_rows(png_const_structrp png_ptr,
                      png_inforp info_ptr,
                      png_bytepp row_pointers)
    void png_write_png(png_structrp png_ptr,
                       png_inforp info_ptr,
                       int transforms,
                       png_voidp params)
    void png_destroy_write_struct(png_structpp png_ptr_ptr,
                                  png_infopp info_ptr_ptr)

    # PNG optimization options
    void png_set_compression_level(png_structrp png_ptr,
                                   int level)
    void png_set_compression_mem_level(png_structrp png_ptr,
                                       int mem_level)
    void png_set_compression_strategy(png_structrp png_ptr,
                                      int strategy)
    void png_set_filter(png_structrp png_ptr,
                        int method,
                        int filters)

    # Buffer writing
    int png_image_write_to_memory(png_imagep image,
                                  void *memory,
                                  png_alloc_size_t *memory_bytes,
                                  int convert_to_8_bit,
                                  const void *buffer,
                                  png_int_32 row_stride,
                                  const void *colormap)
    void png_set_write_fn(png_structp png_ptr,
                          png_voidp io_ptr,
                          png_rw_ptr write_data_fn,
                          png_flush_ptr output_flush_fn)

    # Should not be necessary if png_write_png() works
    void png_write_info(png_structrp png_ptr,
                        png_const_inforp info_ptr)
    void png_write_row(png_structrp png_ptr,
                       png_const_bytep row)
    void png_write_end(png_structrp png_ptr,
                       png_inforp info_ptr)
