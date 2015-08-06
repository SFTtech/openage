// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_COMPRESS_BITSTREAM_H_
#define OPENAGE_UTIL_COMPRESS_BITSTREAM_H_

#include <cstring>

#include <functional>

#include "../../error/error.h"
#include "../compiler.h"


namespace openage {
namespace util {
namespace compress {


/**
 * Type of the 'read' callback function.
 *
 * See lzxd.h for documentation.
 */
using read_callback_t = std::function<size_t (unsigned char *buf, size_t size)>;


/**
 * Bitstream that is stored in 16-bit little-endian numbers.
 * There are two modes:
 *
 * Bit stream
 * ----------
 *
 * This is the default mode.
 * Bits may be retrieved via read_bits().
 *
 * The input bytes
 *
 * 0babcdefgh
 * 0bijklmnop
 * 0bmnopqrst
 * 0buvwxyzAB
 *
 * are decoded to the following bitstream:
 *
 * ijkl mnop abcd efgh uvwx yzAB mnop qrst
 *
 * Someone should get a medal for this.
 *
 * Byte stream
 * -----------
 *
 * In bytestream mode, read_bytes() can be used to read any number of bytes,
 * verbatim, from the data source.
 *
 * Mode switching
 * --------------
 *
 * You can switch between modes via
 *
 *  - switch_to_bytestream_mode()
 *  - switch_to_bitstream_mode()
 *
 * Every modeswitch aligns the stream to a 16-bit boundary by discarding
 * the appropriate amount of nullbits/nullbytes.
 * Calling the modeswitch methods while already in the respective mode does
 * nothing.
 */
template<unsigned int inbuf_size>
class BitStream {
public:
	/**
	 * true if the read callback has returned EOF.
	 */
	unsigned char eof;

private:
	/**
	 * read callback function; invoked whenever the input buffer is empty.
	 */
	read_callback_t read_callback;

	/**
	 * Input byte buffer.
	 * Used by both modes (via ensure_bits and read_bytes).
	 */
	unsigned char inbuf[inbuf_size];

	/**
	 * Pointer to current position in inbuf.
	 */
	unsigned char *i_ptr;

	/**
	 * Pointer to end of valid data in inbuf.
	 */
	unsigned char *i_end;

	/**
	 * Bit buffer; used in bitstream mode.
	 * Filled by via get_next_byte() via ensure_bits(),
	 * read by peak_bits(),
	 * cleared by remove_bits().
	 */
	unsigned int bit_buffer;

	/**
	 * The number of valid bits in bit_buffer.
	 * If e.g. the value is 2, only the two most significant bits of
	 * bit_buffer are set.
	 */
	unsigned int bits_left;

	/**
	 * counts the number of bits (in bitstream mode) or bytes (in bytestream mode),
	 * for determining the correct number of bits/bytes to discard.
	 */
	size_t stream_position;

	/**
	 * If set, the bitstream is currently in bytestream mode.
	 */
	bool bitstream_mode;

	/**
	 * returns the number of bytes that are available in the input byte buffer.
	 */
	unsigned int input_bytes_available() {
		if (unlikely(this->i_ptr > this->i_end)) {
			throw Error(MSG(err) << "input byte buffer state invalid: i_ptr > i_end");
		}

		return this->i_end - this->i_ptr;
	}

	/**
	 * ensures that at least one byte is available in the input byte buffer.
	 *
	 * returns the amount of available bytes.
	 */
	void ensure_input_bytes() {
		// check if we need to actually read some bytes.
		if (this->input_bytes_available() == 0) {
			// fill the entire input buffer.
			size_t read_bytes = this->read_callback(this->inbuf, inbuf_size);

			// we might overrun the input stream by asking for bits we don't use,
			// so fake 2 more bytes at the end of input
			if (unlikely(read_bytes == 0)) {
				if (this->eof) {
					throw Error(MSG(err) << "Unexpected EOF in the middle of a block");
				} else {
					read_bytes = 2;
					this->inbuf[0] = 0;
					this->inbuf[1] = 0;
					this->eof = true;
				}
			}

			if (unlikely(read_bytes > (int) inbuf_size)) {
				throw Error(MSG(err) << "read() returned more data than requested");
			}

			// update i_ptr and i_end
			this->i_ptr = this->inbuf;
			this->i_end = &this->inbuf[read_bytes];
		}

		// check if the reading was successful.
		if (unlikely(this->i_ptr >= this->i_end)) {
			throw Error(MSG(err) << "input byte buffer empty: failed to ensure_input_bytes");
		}
	}

	/*
	 * allow our friend HuffmanTable to directly use ensure_bits, peek_bits and remove_bits.
	 */
	template<unsigned maxsymbols_p, unsigned tablebits_p, bool allow_empty>
	friend class HuffmanTable;

	/**
	 * for use in bitstream mode.
	 *
	 * loads the next 16-bit value into the bitstream.
	 */
	void load_next_16_bits() {
		// example: input stream contains bytes A, B. previous byte was J.
		//
		// 5 bits are left
		// bit buffer is:                     jjjjj000 00000000 00000000 00000000
		//
		// ensure_bits(9) is called.
		// b0 = aaaaaaaa
		// b1 = bbbbbbbb
		// bit_buffer |= bbbbbbbb aaaaaaaa << (32 - 16 - 5) == 11
		//
		// new bit buffer:                    jjjjjbbb bbbbbaaa aaaaa000 00000000

		// read two bytes to b0, b1
		this->ensure_input_bytes();
		unsigned char b0 = *i_ptr++;
		this->ensure_input_bytes();
		unsigned char b1 = *i_ptr++;

		// inject bits into bit_buffer
		bit_buffer |= ((b1 << 8) | b0) << (sizeof(bit_buffer) * 8 - 16 - bits_left);
		bits_left += 16;
	}

	/**
	 * for use in bitstream mode.
	 *
	 * ensures there are at least nbits bits in the bit buffer.
	 */
	inline void ensure_bits(unsigned int nbits) {
		if (unlikely(!this->bitstream_mode)) {
			throw Error(MSG(err) << "instream: attempted to ensure bits while in bytestream mode");
		}

		while (bits_left < nbits) {
			this->load_next_16_bits();
		}
	}

	/**
	 * for use in bitstream mode.
	 *
	 * returns nbits bits from the bit buffer, without removing them.
	 */
	unsigned peek_bits(unsigned int nbits) {
		// example: bit buffer is:   abcdefgh ijkl0000 00000000 00000000
		//
		// peek_bits(3) is called.
		//
		// return (bit_buffer >> (32 - 3) == 29
		//
		// returned value is:        abc
		this->ensure_bits(nbits);

		return (bit_buffer >> (sizeof(bit_buffer) * 8 - nbits));
	}

	/**
	 * for use in bitstream mode.
	 *
	 * removes nbits bits from the bit buffer.
	 */
	void remove_bits(unsigned int nbits) {
		// example: bit buffer is:  abcdefgh ijkl0000 00000000 00000000
		//
		// remove_bits(3) is called.
		//
		// bit_buffer <<= 3
		//
		// resulting bit buffer is: defghijk l0000000 00000000 00000000
		this->ensure_bits(nbits);

		bit_buffer <<= nbits;
		bits_left -= nbits;
		this->stream_position += nbits;
	}

	/**
	 * for use in bitstream mode.
	 *
	 * Aligns the bitstream to the next multiple of 2 bytes.
	 *
	 * If min_discard is given, at least that amount of bits is discarded.
	 */
	void align_bitstream(unsigned int min_discard=0) {
		unsigned int nbits = this->stream_position % 16;
		if (nbits != 0) {
			nbits = 16 - nbits;
		}

		while (unlikely(nbits < min_discard)) {
			nbits += 16;
		}

		if (nbits == 0) {
			return;
		}

		if (this->peek_bits(nbits)) {
			auto errmsg = MSG(err);
			errmsg << "attempted to discard " << nbits << " non-null bits: ";
			for(unsigned ctr = 1; ctr <= nbits; ctr++) {
				errmsg << (this->peek_bits(ctr) & 1);
			}
			throw Error(errmsg);
		}
		this->remove_bits(nbits);
	}

public:
	BitStream(read_callback_t read_callback)
		:
		eof{false},
		read_callback{read_callback},
		i_ptr{inbuf},
		i_end{inbuf},
		bit_buffer{0},
		bits_left{0},
		stream_position{0},
		bitstream_mode{true} {

		static_assert(inbuf_size >= 2, "inbuf size must be at least 2");
		static_assert(inbuf_size % 2 == 0, "inbuf size must be even");
	}

	/**
	 * for use in bitstream mode.
	 *
	 * takes nbits bits from the bit buffer and returns them.
	 */
	unsigned read_bits(unsigned int nbits) {
		unsigned ret = peek_bits(nbits);
		remove_bits(nbits);
		return ret;
	}

	/**
	 * for use in bytestream mode.
	 *
	 * reads up to count verbatim bytes from the input byte buffer,
	 * and writes them to *buf.
	 *
	 * reads at least 1 and at most count bytes.
	 */
	unsigned read_bytes(unsigned char *buf, unsigned count) {
		if (unlikely(this->bitstream_mode == true)) {
			throw Error(MSG(err) << "attempt to read_bytes while in bitstream mode");
		}

		this->ensure_input_bytes();

		unsigned int available = this->input_bytes_available();
		if (available < count) {
			count = available;
		}

		memcpy(buf, this->i_ptr, count);
		this->i_ptr += count;
		this->stream_position += count;

		return count;
	}

	/**
	 * for use in bytestream mode.
	 *
	 * reads a single verbatim byte from the input byte buffer, and returns it.
	 */
	unsigned char read_single_byte() {
		unsigned char buf;

		if (unlikely(this->read_bytes(&buf, 1) != 1)) {
			throw Error(MSG(err) << "failed to read single byte in bytestream mode");
		}

		return buf;
	}

	/**
	 * for use in bytestream mode.
	 *
	 * reads a little-endian-encoded 4-byte number and returns it.
	 */
	unsigned int read_4bytes_le() {
		unsigned int result;

		result  = this->read_single_byte() <<  0;
		result |= this->read_single_byte() <<  8;
		result |= this->read_single_byte() << 16;
		result |= this->read_single_byte() << 24;

		return result;
	}

	/**
	 * Switches to bitstream mode.
	 *
	 * If the previous bytestream had an odd size, one byte is discarded first.
	 */
	void switch_to_bitstream_mode() {
		if (this->bitstream_mode == true) {
			return;
		}

		if (this->stream_position & 1) {
			// bytestream hat odd length; discard one nullbyte.
			unsigned char data = this->read_single_byte();
			if (data != 0) {
				throw Error(MSG(err).fmt("attempted to discard a non-zero byte at end of bytestream: %02x", data));
			}
		}

		this->bitstream_mode = true;
		this->stream_position = 0;
	}

	/**
	 * Switches to bytestream mode.
	 *
	 * Discards 1 to 16 bits to align the bitstream first.
	 */
	void switch_to_bytestream_mode() {
		if (this->bitstream_mode == false) {
			return;
		}

		// for some weird reason, the alignment padding here is 1 to 16 bits, not 0 to 15.
		// thus, discard an additional bit.
		this->align_bitstream(1);

		this->bitstream_mode = false;
		this->stream_position = 0;

		if (this->bits_left) {
			throw Error(MSG(err) << "bits left after switching to bytestream mode: " << this->bits_left);
		}
	}

	/**
	 * Aligns the bitstream - that is, _if_ we're currenlty in bitstream mode.
	 *
	 * Otherwise, a no-op.
	 */
	void align_if_in_bitstream_mode() {
		if (this->bitstream_mode == true) {
			this->align_bitstream();
		}
	}
};


}}} // openage::util::compress

#endif
