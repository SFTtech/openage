// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdlib>

// pxd: from libopenage.pyinterface.functional cimport Func2
#include "bitstream.h"

namespace openage {
namespace util {
namespace compress {


// pxd: const unsigned LZX_FRAME_SIZE
constexpr unsigned LZX_FRAME_SIZE = 32768;


/**
 * Wraps a LZXDStream object by reference; exposing LZXDStream directly would
 * cause too much header pollution due to its many member objects, and their
 * member objects.
 *
 * pxd:
 *
 * cppclass LZXDecompressor:
 *     LZXDecompressor(
 *         Func2[size_t, unsigned char *, size_t],
 *         unsigned int window_bits,
 *         unsigned int reset_interval
 *     ) except +
 *
 *     unsigned decompress_next_frame(unsigned char *out_buf) except +
 */
class OAAPI LZXDecompressor {
public:
	/*
	 * Initialises LZX decompression state for decoding an LZX stream.
	 *
	 * @param read_callback      a callback method that is invoked whenever the decompressor
	 *                           requires more data on the input stream.
	 *                           first argument: buf (writable buffer),
	 *                           second argument: size (amount of data requested).
	 *                           must return: 0 on EOF, and 1 to size otherwise.
	 *                           must fill buf [result] bytes.
	 *                           must throw an exception on error.
	 * @param window_bits        the size of the decoding window, which must be
	 *                           between 15 and 21 inclusive.
	 * @param reset_interval     the interval at which the LZX bitstream is
	 *                           reset, in multiples of LZX frames (32678
	 *                           bytes), e.g. a value of 2 indicates the input
	 *                           stream resets after every 65536 output bytes.
	 *                           A value of 0 indicates that the bitstream never
	 *                           resets, such as in CAB LZX streams.
	 */
	LZXDecompressor(read_callback_t read_callback,
	                unsigned int window_bits=21,
	                unsigned int reset_interval=0);

	/**
	 * Frees the internally-allocated LZXDStream object.
	 */
	~LZXDecompressor();

	/**
	 * Decompresses a single 32-KiB frame.
	 * Data from the input stream is read as required from read_callback.
	 * The decoded frame is written to output_buf.
	 * output_buf must be at least LZX_FRAME_SIZE (32768) bytes in size.
	 *
	 * Returns 0 in case of EOF, and the size of the frame otherwise.
	 *
	 * On error, an exception is thrown. After that, the object shall
	 * be destroyed; every other operation on it may invoke undefined
	 * behavior.
	 */
	unsigned int decompress_next_frame(unsigned char *output_buf);

private:
	class LZXDStream *stream;

	// don't even think about it.
	LZXDecompressor(const LZXDecompressor &other) = delete;
	LZXDecompressor(LZXDecompressor &&other) = delete;

	LZXDecompressor &operator =(const LZXDecompressor &other) = delete;
	LZXDecompressor &operator =(LZXDecompressor &&other) = delete;
};

}}} // openage::util::compress
