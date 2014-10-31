// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef PYEXT_OPENAGE_CONVERT_CABEXTRACT_LZXD_LZXD_H_
#define PYEXT_OPENAGE_CONVERT_CABEXTRACT_LZXD_LZXD_H_

#include <cstdio>

class IOWrapper {
public:
	/**
	 * feeds data to the decompressor
	 * on error, needs to return -1
	 * on EOF, return 0
	 * else, block until data is available, and return number of bytes.
	 */
	virtual ssize_t read(void *buf, size_t size) = 0;

	/**
	 * receives decompressed data from the decompressor.
	 * must read all bytes (size),
	 * return -1 otherwise.
	 */
	virtual ssize_t write(const void *buf, size_t size) = 0;

	virtual ~IOWrapper() {};
};

/**
 * lzx decompression highlevel function.
 *
 * allows exporting the functionality without actually exposing the
 * LZXDStream and HuffmanTable classes in this header.
 *
 * for documentation, see LZXDStream constructor documentation in lzxd.cpp.
 */
void decompress(unsigned window_bits, unsigned reset_interval, unsigned input_buffer_size, off_t output_length, IOWrapper *iowrapper);

#endif
