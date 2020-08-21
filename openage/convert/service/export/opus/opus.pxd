# Copyright 2018-2020 the openage authors. See copying.md for legal info.

cdef extern from "opus/opus.h":
    ctypedef struct OpusEncoder:
        pass

    ctypedef int opus_int32
    ctypedef short opus_int16
    OpusEncoder *opus_encoder_create(opus_int32 Fs, int channels,
                                     int application, int *error)
    void opus_encoder_destroy(OpusEncoder *st)
    opus_int32 opus_encode(OpusEncoder *st, const opus_int16 *pcm,
                           int frame_size, unsigned char *data,
                           opus_int32 max_data_bytes)
    int opus_encoder_ctl(OpusEncoder *st, int request)

cdef extern from "opus/opus_defines.h":
    # Errors
    const int OPUS_OK = 0
    const int OPUS_ALLOC_FAIL = -7
    # Application
    const int OPUS_APPLICATION_AUDIO = 2049
    # Macros
    int OPUS_GET_LOOKAHEAD(opus_int32 *x)
