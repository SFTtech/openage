#ifndef _OPENAGE_CONVERT_LZXD_LZXD_H_
#define _OPENAGE_CONVERT_LZXD_LZXD_H_

#include <cstdio>

/**
 * lzx decompression highlevel function.
 *
 * allows exporting the functionality without actually exposing the
 * LZXDStream and HuffmanTable classes in this header.
 *
 * for documentation, see LZXDStream constructor documentation in lzxd.cpp.
 */
void decompress(unsigned window_bits, unsigned reset_interval, unsigned input_buffer_size, off_t output_length);

#endif //_OPENAGE_CONVERT_LZXD_LZXD_H_
