# Copyright 2018-2018 the openage authors. See copying.md for legal info.

cdef extern from "ogg/config_types.h":
    ctypedef short ogg_int16_t
    ctypedef long ogg_int64_t

cdef extern from "ogg/ogg.h":
    ctypedef struct ogg_stream_state:
        pass
    ctypedef struct ogg_page:
        unsigned char *header
        long header_len
        unsigned char *body
        long body_len
    ctypedef struct ogg_packet:
        unsigned char *packet
        long bytes
        long b_o_s
        long e_o_s

        ogg_int64_t granulepos
        ogg_int64_t packetno

    int ogg_stream_packetin(ogg_stream_state *os, ogg_packet *op)
    int ogg_stream_pageout(ogg_stream_state *os, ogg_page *og)
    int ogg_stream_flush(ogg_stream_state *os, ogg_page *og)

    int ogg_stream_init(ogg_stream_state *os, int serialno)
    int ogg_stream_clear(ogg_stream_state *os)
