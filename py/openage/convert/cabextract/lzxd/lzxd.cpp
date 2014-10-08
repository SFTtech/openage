/*
 * this code is taken from the cabextract project <http://www.cabextract.org.uk/>.
 *
 * the original code was horribly raped; all of the macros and optimizations were
 * ripped out and replaced by c++11 classes with _templates_.
 *
 * the original author is probably weeping in pain.
 * sorry, your work was awesome, but _I_ find it easier to read that way :P
 *
 * TODO list:
 *
 * - make it so the decompressing function is called once for every megabyte or so, instead of once for everything
 * - understand and abstract away the intel D8 post-processing. this should make the code/interface much cleaner
 * - save the state between every megabyte, effectively allowing seeking
 * - actually understand what the code does
 * - rewrite it (maybe in python)
 */

#include "lzxd.h"

#include <cerrno>
#include <cstdarg>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "util.h"

// LZX specification constants
constexpr unsigned LZX_MIN_MATCH = 2;
//constexpr unsigned LZX_MAX_MATCH = 257;             // seems to be unused. I'm scared.
constexpr unsigned LZX_NUM_CHARS = 256;
constexpr unsigned LZX_BLOCKTYPE_INVALID = 0;         // also blocktypes 4-7 invalid
constexpr unsigned LZX_BLOCKTYPE_VERBATIM = 1;
constexpr unsigned LZX_BLOCKTYPE_ALIGNED = 2;
constexpr unsigned LZX_BLOCKTYPE_UNCOMPRESSED = 3;
constexpr unsigned LZX_PRETREE_NUM_ELEMENTS = 20;
constexpr unsigned LZX_ALIGNED_NUM_ELEMENTS = 8;
constexpr unsigned LZX_NUM_PRIMARY_LENGTHS = 7;       // missing from spec!
constexpr unsigned LZX_NUM_SECONDARY_LENGTHS = 249;   // length tree #elements

// LZX huffman constants: tweak tablebits as desired
constexpr unsigned LZX_PRETREE_MAXSYMBOLS = LZX_PRETREE_NUM_ELEMENTS;
constexpr unsigned LZX_PRETREE_TABLEBITS = 6;
constexpr unsigned LZX_MAINTREE_MAXSYMBOLS = LZX_NUM_CHARS + 50 * 8;
constexpr unsigned LZX_MAINTREE_TABLEBITS = 12;
constexpr unsigned LZX_LENGTH_MAXSYMBOLS = LZX_NUM_SECONDARY_LENGTHS + 1;
constexpr unsigned LZX_LENGTH_TABLEBITS = 12;
constexpr unsigned LZX_ALIGNED_MAXSYMBOLS = LZX_ALIGNED_NUM_ELEMENTS;
constexpr unsigned LZX_ALIGNED_TABLEBITS = 7;
constexpr unsigned LZX_LENTABLE_SAFETY = 64;          // table decoding overruns are allowed

constexpr unsigned LZX_FRAME_SIZE = 32768;            // the size of a frame in LZX

/**
 * LZX static data tables:
 *
 * LZX uses 'position slots' to represent match offsets. For every match,
 * a small 'position slot' number and a small offset from that slot are
 * encoded instead of one large offset.
 *
 * position_base[] is an index to the position slot bases
 *
 * extra_bits[] states how many bits of offset-from-base data is needed.
 *
 * They are generated like so:
 * for (i = 0;  i < 4; i++) extra_bits[i] = 0;
 * for (i = 4,  j = 0; i < 36; i+=2) extra_bits[i] = extra_bits[i+1] = j++;
 * for (i = 36; i < 51; i++) extra_bits[i] = 17;
 * for (i = 0,  j = 0; i < 51; j += 1 << extra_bits[i++]) position_base[i] = j;
 */

static const unsigned int position_base[] = {
	0, 1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128, 192, 256,
	384, 512, 768, 1024, 1536, 2048, 3072, 4096, 6144, 8192, 12288,
	16384, 24576, 32768, 49152, 65536, 98304, 131072, 196608, 262144,
	393216, 524288, 655360, 786432, 917504, 1048576, 1179648, 1310720,
	1441792, 1572864, 1703936, 1835008, 1966080, 2097152
};
static const unsigned char extra_bits[] = {
	0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8,
	9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16,
	17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17
};

class LZXDStream;

class BitBuffer {
private:
	unsigned char *inbuf;
	unsigned int inbuf_size;
	unsigned char *i_ptr;
	unsigned char *i_end;
	unsigned int bits_left;
	unsigned char input_end;
	uint64_t bit_position;
	LZXDStream *lzx;

public:
	unsigned int bit_buffer;

	BitBuffer(LZXDStream *lzx, unsigned inbuf_size)
			:
			inbuf_size{inbuf_size},
			bits_left{0},
			input_end{0},
			bit_position{0},
			lzx{lzx},
			bit_buffer{0}
	{
		if (inbuf_size % 2 != 0) {
			inbuf_size += 1;
		}
		if (inbuf_size < 2) {
			throwerr("minimum inbuf size: 2 bytes!");
		}
		inbuf = new unsigned char[inbuf_size];
		i_ptr = inbuf;
		i_end = inbuf;
	}

private:
	// re-fills the input buf from the input file
	void read_input();

	// returns the next byte from the input buf
	unsigned char read_byte() {
		// if the byte buffer is empty, call read_input
		if (i_ptr >= i_end) {
			read_input();
		}
		return *i_ptr++;
	}

public:
	// ensures there are at least N bits in the bit buffer
	void ensure_bits(unsigned nbits) {
		while (bits_left < nbits) {
			// read two bytes to b0, b1
			unsigned char b0 = read_byte();
			unsigned char b1 = read_byte();
			// inject bits
			bit_buffer |= ((b1 << 8) | b0) << (sizeof(bit_buffer) * 8 - 16 - bits_left);
			bits_left += 16;
		}
	}

	// extracts without removing N bits from the bit buffer
	// warning: call ensure_bits first!
	unsigned int peek_bits(unsigned nbits) {
		return (bit_buffer >> (sizeof(bit_buffer) * 8 - nbits));
	}

	// removes N bits from the bit buffer
	// warning: call ensure_bits first!
	void remove_bits(unsigned nbits) {
		bit_buffer <<= nbits;
		bits_left -= nbits;
		bit_position += nbits;
	}

	// takes the next few bits from the bitstream and returns them
	unsigned read_bits(unsigned nbits) {
		ensure_bits(nbits);
		unsigned ret = peek_bits(nbits);
		remove_bits(nbits);
		return ret;
	}

	// skips the next nbits in the stream
	void skip_bits(unsigned nbits) {
		ensure_bits(nbits);
		remove_bits(nbits);
	}

	// aligns the bitstream to the next multiple of 2 bytes
	void align_to_short() {
		unsigned nbits = bit_position % 16;
		if (nbits != 0) {
			nbits = 16 - nbits;
		}
		skip_bits(nbits);
	}
};

template<unsigned maxsymbols_p, unsigned tablebits_p, bool allow_empty=false>
class HuffmanTable {
private:
	LZXDStream *lzx;

public:
	static constexpr unsigned HUFF_MAXBITS = 16;
	static constexpr unsigned maxsymbols = maxsymbols_p;
	static constexpr unsigned tablebits = tablebits_p;

	// table of code lengths (in bits) for each symbol
	unsigned char len[maxsymbols_p + LZX_LENTABLE_SAFETY];

	// table to fill up with decoded values for symbols, and pointers.
	// symbols shorter than tablebits_p are decoded by direct lookup
	// (that's the first (1<<tablebits_p) entries).
	// longer symbols are decoded by following pointers to the appropriate
	// leaf in the upper nsyms*2 fields.
	uint16_t table[(1 << tablebits_p) + (maxsymbols_p * 2)];

	// the table could not be constructed and is empty (try_make_decode_table failed).
	// this will lead to errors, should read_sym() be called.
	bool is_empty;

	HuffmanTable(LZXDStream *lzx) : lzx{lzx}, is_empty{true} {}

	// Decodes the next huffman symbol from the input bitstream,
	// and returns it.
	// Do not use this function on a table unless build_decode_table() succeeded.
	int read_sym();

	// calls try_make_decode_table, and, if allow_empty is false,
	// raises an exception on failure.
	// builds a huffman lookup table from code lengths.
	void make_decode_table();

	// reads in code lengths for symbols first to last in the given table.
	// The code lengths are stored in their own special LZX way.
	void read_lengths(unsigned int first, unsigned int last);
private:
	// This function was originally coded by David Tritscher.
	// It builds a fast huffman decoding table from
	// a canonical huffman code lengths table.
	bool try_make_decode_table();
};


class LZXDStream {
public:
	off_t offset;                   // number of bytes actually output
	off_t length;                   // overall decompressed length of stream

	unsigned char *window;          // decoding window
	unsigned int   window_size;     // window size
	unsigned int   window_posn;     // decompression offset within window
	unsigned int   frame_posn;      // current frame offset within in window
	unsigned int   frame;           // the number of 32kb frames processed
	unsigned int   reset_interval;  // which frame do we reset the compressor?

	unsigned int   R0, R1, R2;      // for the LRU offset system
	unsigned int   block_length;    // uncompressed length of this LZX block
	unsigned int   block_remaining; // uncompressed bytes still left to decode

	signed int     intel_filesize;  // magic header value used for transform
	signed int     intel_curpos;    // current offset in transform space

	unsigned char  block_type;      // type of the current block
	unsigned char  header_read;     // have we started decoding at all yet?
	unsigned char  posn_slots;      // how many posn slots in stream?
	unsigned char  input_end;       // have we reached the end of input?

	IOWrapper *iowrapper;

	// IO buffering
	BitBuffer bits;
	unsigned char *o_ptr, *o_end;

	// huffman code lenghts
	HuffmanTable<LZX_PRETREE_MAXSYMBOLS, LZX_PRETREE_TABLEBITS> htpre;
	HuffmanTable<LZX_MAINTREE_MAXSYMBOLS, LZX_MAINTREE_TABLEBITS> htmain;
	HuffmanTable<LZX_LENGTH_MAXSYMBOLS, LZX_LENGTH_TABLEBITS, true> htlength;
	HuffmanTable<LZX_ALIGNED_MAXSYMBOLS, LZX_ALIGNED_TABLEBITS> htaligned;

	// used purely for doing the intel E8 transform
	unsigned char e8_buf[LZX_FRAME_SIZE];

	void set_output_length(off_t out_bytes) {
		// see description of output_length for lzxd_init()
		length = out_bytes;
	}

	/**
	 * Initialises LZX decompression state for decoding an LZX stream.
	 *
	 * @param window_bits        the size of the decoding window, which must be
	 *                           between 15 and 21 inclusive.
	 * @param reset_interval     the interval at which the LZX bitstream is
	 *                           reset, in multiples of LZX frames (32678
	 *                           bytes), e.g. a value of 2 indicates the input
	 *                           stream resets after every 65536 output bytes.
	 *                           A value of 0 indicates that the bitstream never
	 *                           resets, such as in CAB LZX streams.
	 * @param input_buffer_size  the number of bytes to use as an input
	 *                           bitstream buffer.
	 * @param output_length      the length in bytes of the entirely
	 *                           decompressed output stream, if known in
	 *                           advance. It is used to correctly perform the
	 *                           Intel E8 transformation, which must stop 6
	 *                           bytes before the very end of the
	 *                           decompressed stream. It is not otherwise used
	 *                           or adhered to. If the full decompressed
	 *                           length is known in advance, set it here.
	 *                           If it is NOT known, use the value 0, and call
	 *                           lzxd_set_output_length() once it is
	 *                           known. If never set, 4 of the final 6 bytes
	 *                           of the output stream may be incorrect.
	 * @param iowrapper          used for reading/writing compressed/decompressed
	 *                           data.
	 */
	LZXDStream(unsigned window_bits, unsigned reset_interval, unsigned input_buffer_size, off_t output_length, IOWrapper *iowrapper);

	/**
	 * Decompresses entire or partial LZX streams.
	 *
	 * The number of bytes of data that should be decompressed is given as the
	 * out_bytes parameter. If more bytes are decoded than are needed, they
	 * will be kept over for a later invocation.
	 *
	 * The output bytes will be passed to the system->write() function given in
	 * lzxd_init(), using the output file handle given in lzxd_init(). More than
	 * one call may be made to system->write().
	 *
	 * Input bytes will be read in as necessary using the system->read()
	 * function given in lzxd_init(), using the input file handle given in
	 * lzxd_init(). This will continue until system->read() returns 0 bytes,
	 * or an error. Errors will be passed out of the function as
	 * MSPACK_ERR_READ errors. Input streams should convey an "end of input
	 * stream" by refusing to supply all the bytes that LZX asks for when they
	 * reach the end of the stream, rather than return an error code.
	 *
	 * @param out_bytes the number of bytes of (plain) data to decompress.
	 *
	 * on error, an exception is thrown, and lzx should be considered unusable.
	 */
	void decompress(off_t out_bytes);

	void reset_state();
};

void BitBuffer::read_input() {
	int read = this->lzx->iowrapper->read(inbuf, inbuf_size);
	if (read < 0) {
		throwerr("could not read %d", errno);
	}

	// we might overrun the input stream by asking for bits we don't use,
	// so fake 2 more bytes at the end of input
	if (read == 0) {
		if (input_end) {
			throwerr("out of input bytes");
		} else {
			read = 2;
			inbuf[0] = 0;
			inbuf[1] = 0;
			input_end = 1;
		}
	}

	// update i_ptr and i_end
	i_ptr = inbuf;
	i_end = &inbuf[read];
}

template<unsigned maxsymbols_p, unsigned tablebits_p, bool allow_empty>
int HuffmanTable<maxsymbols_p, tablebits_p, allow_empty>::read_sym() {
	lzx->bits.ensure_bits(HUFF_MAXBITS);
	uint16_t sym = table[lzx->bits.peek_bits(tablebits)];

	unsigned i = 1 << (sizeof(lzx->bits.bit_buffer) * 8 - tablebits);
	while (sym >= maxsymbols) {
		// huff_traverse
		if ((i >>= 1) == 0) {
			throwerr("huff_error in huff_traverse");
		}
		sym = table[(sym << 1) | ((lzx->bits.bit_buffer & i) ? 1 : 0)];
	}

	lzx->bits.remove_bits(len[sym]);
	return sym;
}

template<unsigned maxsymbols_p, unsigned tablebits_p, bool allow_empty>
void HuffmanTable<maxsymbols_p, tablebits_p, allow_empty>::make_decode_table() {
	if (try_make_decode_table()) {
		is_empty = false;
	} else {
		if (allow_empty) {
			// allow an empty tree, but don't decode symbols with it

			// if any of the symbols has a greater-than-zero length, fail.
			for (unsigned i = 0; i < maxsymbols; i++) {
				if (len[i] > 0) {
					throwerr("failed to build table that would allow_empty");
				}
			}
			is_empty = true;
		} else {
			throwerr("make_decode_table failed");
		}
	}
}

template<unsigned maxsymbols_p, unsigned tablebits_p, bool allow_empty>
bool HuffmanTable<maxsymbols_p, tablebits_p, allow_empty>::try_make_decode_table() {
	uint16_t sym, next_symbol;
	unsigned int leaf, fill;
	unsigned char bit_num;
	unsigned int pos = 0; // the current position in the decode table
	unsigned int table_mask = 1 << tablebits;
	unsigned int bit_mask = table_mask >> 1; // don't do 0 length codes

	// fill entries for codes short enough for a direct mapping
	for (bit_num = 1; bit_num <= tablebits; bit_num++) {
		for (sym = 0; sym < maxsymbols; sym++) {
			if (len[sym] != bit_num) {
				continue;
			}
			leaf = pos;
			if((pos += bit_mask) > table_mask) {
				return false; // table overrun
			}

			// fill all possible lookups of this symbol with the symbol itself
			for (fill = bit_mask; fill-- > 0;) {
				table[leaf++] = sym;
			}
		}
		bit_mask >>= 1;
	}

	// exit with success if table is now complete
	if (pos == table_mask) {
		return true;
	}

	// mark all remaining table entries as unused
	for (sym = pos; sym < table_mask; sym++) {
		table[sym] = 0xFFFF;
	}

	// next_symbol = base of allocation for long codes
	next_symbol = ((table_mask >> 1) < maxsymbols) ? maxsymbols : (table_mask >> 1);

	// give ourselves room for codes to grow by up to 16 more bits.
	// codes now start at bit (tablebits + 16) and end at (tablebits + 16 - codelength)
	pos <<= 16;
	table_mask <<= 16;
	bit_mask = 1 << 15;

	for (bit_num = tablebits + 1; bit_num <= HUFF_MAXBITS; bit_num++) {
		for (sym = 0; sym < maxsymbols; sym++) {
			if (len[sym] != bit_num) {
				continue;
			}

			leaf = pos >> 16;
			for (fill = 0; fill < (bit_num - tablebits); fill++) {
				// if this path hasn't been taken yet, 'allocate' two entries
				if (table[leaf] == 0xFFFF) {
						table[(next_symbol << 1) + 0] = 0xFFFF;
						table[(next_symbol << 1) + 1] = 0xFFFF;
						table[leaf] = next_symbol++;
				}

				// follow the path and select either left or right for next bit
				leaf = table[leaf] << 1;
				if ((pos >> (15-fill)) & 1) leaf++;
			}
			table[leaf] = sym;

			if ((pos += bit_mask) > table_mask) {
				// table overflow
				return false;
			}
		}

		bit_mask >>= 1;
	}

	// full table?
	return (pos == table_mask) ? true : false;
}

template<unsigned maxsymbols_p, unsigned tablebits_p, bool allow_empty>
void HuffmanTable<maxsymbols_p, tablebits_p, allow_empty>::read_lengths(unsigned int first, unsigned int last) {
	// bit buffer and huffman symbol decode variables
	unsigned int x, y;
	int z;

	// read lengths for pretree (20 symbols, lengths stored in fixed 4 bits)
	for (x = 0; x < 20; x++) {
		y = lzx->bits.read_bits(4);
		lzx->htpre.len[x] = y;
	}
	lzx->htpre.make_decode_table();

	for (x = first; x < last; ) {
		z = lzx->htpre.read_sym();
		if (z == 17) {
			// code = 17, run of ([read 4 bits]+4) zeros
			y = lzx->bits.read_bits(4);
			y += 4;
			while (y--) {
				len[x++] = 0;
			}
		}
		else if (z == 18) {
			// code = 18, run of ([read 5 bits]+20) zeros
			y = lzx->bits.read_bits(5);
			y += 20;
			while (y--) {
				len[x++] = 0;
			}
		}
		else if (z == 19) {
			// code = 19, run of ([read 1 bit]+4) [read huffman symbol]
			y = lzx->bits.read_bits(1);
			y += 4;
			z = lzx->htpre.read_sym();
			z = len[x] - z;
			if (z < 0) {
				z += 17;
			}
			while (y--) {
				len[x++] = z;
			}
		}
		else {
			// code = 0 to 16, delta current length entry
			z = len[x] - z; if (z < 0) z += 17;
			len[x++] = z;
		}
	}
}

LZXDStream::LZXDStream(unsigned window_bits, unsigned reset_interval, unsigned input_buffer_size, off_t output_length, IOWrapper *iowrapper)
		:
		offset{0},
		length{output_length},
		window_size{(unsigned) 1 << window_bits},
		window_posn{0},
		frame_posn{0},
		frame{0},
		reset_interval{reset_interval},
		intel_filesize{0},
		intel_curpos{0},
		iowrapper{iowrapper},
		bits{this, input_buffer_size},
		htpre{this},
		htmain{this},
		htlength{this},
		htaligned{this}
{
	// LZX supports window sizes of 2^15 (32Kb) through 2^21 (2Mb)
	if (window_bits < 15 || window_bits > 21) {
		throwerr("bad requested window size: 2^%d bytes", window_bits);
	}

	// allocate decompression window
	window = new unsigned char[window_size];

	// window bits:    15  16  17  18  19  20  21
	// position slots: 30  32  34  36  38  42  50
	posn_slots = 2 * window_bits;
	if (window_bits == 21) {
		posn_slots = 50;
	}
	if (window_bits == 20) {
		posn_slots = 42;
	}

	o_ptr = o_end = &e8_buf[0];
	reset_state();
}

void LZXDStream::reset_state() {
	R0 = 1;
	R1 = 1;
	R2 = 1;
	header_read = 0;
	block_remaining = 0;
	block_type = LZX_BLOCKTYPE_INVALID;

	// initialise tables to 0 (because deltas will be applied to them)
	for (unsigned i = 0; i < LZX_MAINTREE_MAXSYMBOLS; i++) {
		htmain.len[i] = 0;
	}

	for (unsigned i = 0; i < LZX_LENGTH_MAXSYMBOLS; i++) {
		htlength.len[i]   = 0;
	}
}

void LZXDStream::decompress(off_t out_bytes) {
	// bitstream and huffman reading variables
	int i = 0;

	int match_length, length_footer, extra, verbatim_bits, bytes_todo;
	int this_run, main_element, aligned_bits, j;
	unsigned char *runsrc, *rundest, buf[12];
	unsigned int frame_size = 0, end_frame, match_offset;

	if (out_bytes < 0) {
		throwerr("outbytes must be > 0");
	}

	// flush out any stored-up bytes before we begin
	i = this->o_end - this->o_ptr;
	if ((off_t) i > out_bytes) {
		i = (int) out_bytes;
	}
	if (i) {
		if (this->iowrapper->write(this->o_ptr, i) != i) {
			throwerr("could not write all %d bytes", i);
		}
		this->o_ptr += i;
		this->offset += i;
		out_bytes  -= i;
	}
	if (out_bytes == 0) {
		return;
	}

	end_frame = (unsigned int)((this->offset + out_bytes) / LZX_FRAME_SIZE) + 1;

	while (this->frame < end_frame) {
		// have we reached the reset interval? (if there is one?)
		if (this->reset_interval && ((this->frame % this->reset_interval) == 0)) {
			if (this->block_remaining) {
				throwerr("%d bytes remaining at reset interval", this->block_remaining);
			}

			// re-read the intel header and reset the huffman lengths
			reset_state();
		}

		// read header if necessary
		if (!this->header_read) {
			// read 1 bit. if bit=0, intel filesize = 0.
			// if bit=1, read intel filesize (32 bits)
			j = 0;
			i = this->bits.read_bits(1);
			if (i) {
				i = this->bits.read_bits(16);
				j = this->bits.read_bits(16);
			}
			this->intel_filesize = (i << 16) | j;
			this->header_read = 1;
		}

		// calculate size of frame: all frames are 32k except the final frame
		// which is 32kb or less. this can only be calculated when this->length
		// has been filled in.
		frame_size = LZX_FRAME_SIZE;
		if (this->length && (this->length - this->offset) < (off_t)frame_size) {
			frame_size = this->length - this->offset;
		}

		// decode until one more frame is available
		bytes_todo = this->frame_posn + frame_size - window_posn;
		while (bytes_todo > 0) {
			// initialise new block, if one is needed
			if (this->block_remaining == 0) {
				// realign if previous block was an odd-sized UNCOMPRESSED block
				if ((this->block_type == LZX_BLOCKTYPE_UNCOMPRESSED) &&
						(this->block_length & 1)) {
					this->bits.align_to_short();
				}

				// read block type (3 bits) and block length (24 bits)
				this->block_type = this->bits.read_bits(3);
				i = this->bits.read_bits(16);
				j = this->bits.read_bits(8);
				this->block_remaining = this->block_length = (i << 8) | j;

				// read individual block headers
				switch (this->block_type) {
				case LZX_BLOCKTYPE_ALIGNED:
					// read lengths of and build aligned huffman decoding tree
					for (i = 0; i < 8; i++) {
						j = this->bits.read_bits(3);
						this->htaligned.len[i] = j;
					}
					this->htaligned.make_decode_table();
					// no break -- rest of aligned header is same as verbatim
				case LZX_BLOCKTYPE_VERBATIM:
					// read lengths of and build main huffman decoding tree
					this->htmain.read_lengths(0, 256);
					this->htmain.read_lengths(256, LZX_NUM_CHARS + (this->posn_slots << 3));
					this->htmain.make_decode_table();
					// read lengths of and build lengths huffman decoding tree
					this->htlength.read_lengths(0, LZX_NUM_SECONDARY_LENGTHS);
					this->htlength.is_empty = false;
					this->htlength.make_decode_table();
					break;

				case LZX_BLOCKTYPE_UNCOMPRESSED:
					// the LZX stream used for testing this port (from the age2 setup cab)
					// doesn't have any uncompressed blocks, so this case is wholly untested.
					// to emphasize this fact, raise an exception
					// note to future self or other users: when this gets raised, nothing is wrong.
					// just comment it out, but _make sure_ the code actually does the right thing.
					throwerr("untested code: LZX_BLOCKTYPE_UNCOMPRESSED");

					// I'm not precisely sure whether align_to_short does what we need here.
					// align_to_short advances by 0 bits if already aligned, maybe advancing by 16
					// bits would be required.
					this->bits.align_to_short();

					// read 12 bytes of stored R0 / R1 / R2 values
					for (rundest = &buf[0], i = 0; i < 12; i++) {
						*rundest++ = this->bits.read_bits(8);
					}
					R0 = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
					R1 = buf[4] | (buf[5] << 8) | (buf[6] << 16) | (buf[7] << 24);
					R2 = buf[8] | (buf[9] << 8) | (buf[10] << 16) | (buf[11] << 24);
					break;

				default:
					throwerr("decrunch: bad block type %d", this->block_type);
				}
			}

			// decode more of the block:
			// run = min(what's available, what's needed)
			this_run = this->block_remaining;
			if (this_run > bytes_todo) {
				this_run = bytes_todo;
			}

			// assume we decode exactly this_run bytes, for now
			bytes_todo -= this_run;
			this->block_remaining -= this_run;

			// decode at least this_run bytes
			switch (this->block_type) {
			case LZX_BLOCKTYPE_VERBATIM:
				while (this_run > 0) {
					main_element = this->htmain.read_sym();
					if (main_element < (int) LZX_NUM_CHARS) {
						// literal: 0 to LZX_NUM_CHARS-1
						window[window_posn++] = main_element;
						this_run--;
					} else {
						// match: LZX_NUM_CHARS + ((slot<<3) | length_header (3 bits))
						main_element -= LZX_NUM_CHARS;

						// get match length
						match_length = main_element & LZX_NUM_PRIMARY_LENGTHS;
						if (match_length == LZX_NUM_PRIMARY_LENGTHS) {
							if (this->htlength.is_empty) {
								throwerr("decrunch: LENGTH symbol needed but tree is empty");
							}
							length_footer = this->htlength.read_sym();
							match_length += length_footer;
						}
						match_length += LZX_MIN_MATCH;

						// get match offset
						switch ((match_offset = (main_element >> 3))) {
						case 0:
							match_offset = R0;
							break;
						case 1:
							match_offset = R1;
							R1 = R0;
							R0 = match_offset;
							break;
						case 2:
							match_offset = R2;
							R2 = R0;
							R0 = match_offset;
							break;
						case 3:
							match_offset = 1;
							R2 = R1;
							R1 = R0;
							R0 = match_offset;
							break;
						default:
							extra = extra_bits[match_offset];
							verbatim_bits = this->bits.read_bits(extra);
							match_offset = position_base[match_offset] - 2 + verbatim_bits;
							R2 = R1;
							R1 = R0;
							R0 = match_offset;
						}

						if ((window_posn + match_length) > this->window_size) {
							throwerr("decrunch: match ran over window wrap");
						}

						// copy match
						rundest = &window[window_posn];
						i = match_length;
						// does match offset wrap the window?
						if (match_offset > window_posn) {
							// j = length from match offset to end of window
							j = match_offset - window_posn;
							if (j > (int) this->window_size) {
								throwerr("decrunch: match offset beyond window boundaries");
							}
							runsrc = &window[this->window_size - j];
							if (j < i) {
								// if match goes over the window edge, do two copy runs
								i -= j;
								while (j-- > 0) {
									*rundest++ = *runsrc++;
								}
								runsrc = window;
							}
							while (i-- > 0) {
								*rundest++ = *runsrc++;
							}
						} else {
							runsrc = rundest - match_offset;
							while (i-- > 0) {
								*rundest++ = *runsrc++;
							}
						}

						this_run    -= match_length;
						window_posn += match_length;
					}
				} // while (this_run > 0)
				break;
			case LZX_BLOCKTYPE_ALIGNED:
				while (this_run > 0) {
					main_element = this->htmain.read_sym();
					if (main_element < (int) LZX_NUM_CHARS) {
						// literal: 0 to LZX_NUM_CHARS - 1
						window[window_posn++] = main_element;
						this_run--;
					} else {
						// match: LZX_NUM_CHARS + ((slot<<3) | length_header (3 bits))
						main_element -= LZX_NUM_CHARS;

						// get match length
						match_length = main_element & LZX_NUM_PRIMARY_LENGTHS;
						if (match_length == LZX_NUM_PRIMARY_LENGTHS) {
							if (this->htlength.is_empty) {
								throwerr("decrunch: length symbol needed byt tree is empty");
							}
							length_footer = this->htlength.read_sym();
							match_length += length_footer;
						}
						match_length += LZX_MIN_MATCH;

						// get match offset
						switch ((match_offset = (main_element >> 3))) {
						case 0:
							match_offset = R0;
							break;
						case 1:
							match_offset = R1;
							R1 = R0;
							R0 = match_offset;
							break;
						case 2:
							match_offset = R2;
							R2 = R0;
							R0 = match_offset;
							break;
						default:
							extra = extra_bits[match_offset];
							match_offset = position_base[match_offset] - 2;
							if (extra > 3) {
								// verbatim and aligned bits
								extra -= 3;
								verbatim_bits = this->bits.read_bits(extra);
								match_offset += (verbatim_bits << 3);
								aligned_bits = this->htaligned.read_sym();
								match_offset += aligned_bits;
							}
							else if (extra == 3) {
								// aligned bits only
								aligned_bits = this->htaligned.read_sym();
								match_offset += aligned_bits;
							}
							else if (extra > 0) {
								// extra==1, extra==2
								// verbatim bits only
								verbatim_bits = this->bits.read_bits(extra);
								match_offset += verbatim_bits;
							}
							else {
								// extra == 0
								// ??? not defin-ed in LZX specification!
								match_offset = 1;
							}

							// update repeated offset LRU queue
							R2 = R1;
							R1 = R0;
							R0 = match_offset;
						}

						if ((window_posn + match_length) > this->window_size) {
							throwerr("decrunch: match ran over window wrap");
						}

						// copy match
						rundest = &window[window_posn];
						i = match_length;
						// does match offset wrap the window?
						if (match_offset > window_posn) {
							// j = length from match offset to end of window
							j = match_offset - window_posn;
							if (j > (int) this->window_size) {
								throwerr("decrunch: match offset beyond window boundaries");
							}
							runsrc = &window[this->window_size - j];
							if (j < i) {
								// if match goes over the window edge, do two copy runs
								i -= j;
								while (j-- > 0) {
									*rundest++ = *runsrc++;
								}
								runsrc = window;
							}
							while (i-- > 0) {
								*rundest++ = *runsrc++;
							}
						} else {
							runsrc = rundest - match_offset;
							while (i-- > 0) {
								*rundest++ = *runsrc++;
							}
						}

						this_run    -= match_length;
						window_posn += match_length;
					}
				} // while (this_run > 0)
				break;
			default:
				throwerr("this->blocktype neither verbatim nor aligned");
			}

			// did the final match overrun our desired this_run length?
			if (this_run < 0) {
				if ((unsigned int)(-this_run) > this->block_remaining) {
					throwerr("decrunch: overrun went past end of block by %d (%d remaining)", -this_run, this->block_remaining);
				}
				this->block_remaining -= -this_run;
			}
		} // while (bytes_todo > 0)

		// streams don't extend over frame boundaries
		if ((window_posn - this->frame_posn) != frame_size) {
			throwerr("decrunch: decode beyond output frame limits! %d != %d", window_posn - this->frame_posn, frame_size);
		}

		this->bits.align_to_short();

		// check that we've used all of the previous frame first
		if (this->o_ptr != this->o_end) {
			throwerr("%ld avail bytes, new %d frame", this->o_end - this->o_ptr, frame_size);
		}

		// does this intel block _really_ need decoding?

		// analysis:
		// the frame_size check is to prevent errors with the last block.
		// the other two conditions are never false.
		// intel_started was just a perf optimization and was thus removed.
		if (this->intel_filesize && (this->frame <= 32768) && (frame_size > 10)) {
			unsigned char *data = &this->e8_buf[0];
			unsigned char *dataend = &this->e8_buf[frame_size - 10];
			signed int curpos = this->intel_curpos;
			signed int filesize = this->intel_filesize;
			signed int abs_off, rel_off;

			// copy e8 block to the e8 buffer and tweak if needed
			this->o_ptr = data;
			memcpy(data, &this->window[this->frame_posn], frame_size);

			while (data < dataend) {
				if (*data++ != 0xE8) {
					curpos++;
					continue;
				}
				abs_off = data[0] | (data[1]<<8) | (data[2]<<16) | (data[3]<<24);
				if ((abs_off >= -curpos) && (abs_off < filesize)) {
					rel_off = (abs_off >= 0) ? abs_off - curpos : abs_off + filesize;
					data[0] = (unsigned char) rel_off;
					data[1] = (unsigned char) (rel_off >> 8);
					data[2] = (unsigned char) (rel_off >> 16);
					data[3] = (unsigned char) (rel_off >> 24);
				}
				data += 4;
				curpos += 5;
			}

			this->intel_curpos += frame_size;
		} else {
			this->o_ptr = &this->window[this->frame_posn];
			if (this->intel_filesize) {
				this->intel_curpos += frame_size;
			}
		}
		this->o_end = &this->o_ptr[frame_size];

		// write a frame
		i = (out_bytes < (off_t)frame_size) ? (unsigned int)out_bytes : frame_size;
		if (this->iowrapper->write(this->o_ptr, i) != i) {
			throwerr("could not write all %d bytes", i);
		}
		this->o_ptr += i;
		this->offset += i;
		out_bytes -= i;

		// advance frame start position
		this->frame_posn += frame_size;
		this->frame++;

		// wrap window / frame position pointers
		if (window_posn == this->window_size) window_posn = 0;
		if (this->frame_posn == this->window_size) this->frame_posn = 0;

	} // while (this->frame < end_frame)

	if (out_bytes) {
		throwerr("decrunch: %ld bytes left to output", out_bytes);
	}
}


void decompress(unsigned window_bits, unsigned reset_interval, unsigned input_buffer_size, off_t output_length, IOWrapper *iowrapper) {
	LZXDStream s{window_bits, reset_interval, input_buffer_size, output_length, iowrapper};
	s.decompress(output_length);
}
