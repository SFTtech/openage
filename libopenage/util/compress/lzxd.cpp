// This file was adapted from cabextract/libmspack <http://www.cabextract.org.uk/>,
// Copyright 2003-2013 the cabextract contributors.
// It's licensed under the terms of the GNU Library General Public License version 2.
// Modifications Copyright 2014-2019 the openage authors.
// See copying.md for further legal info.

/*
 * the original code was refactored almost beyond recognition;
 * all of the macros and optimizations were ripped out and replaced by
 * templated C++ classes.
 */

#include "lzxd.h"

#include <algorithm>
#include <cinttypes>
#include <cerrno>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>

#include "../../error/error.h"
#include "../compiler.h"

#include "bitstream.h"

namespace openage {
namespace util {
namespace compress {


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


/*
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

static const unsigned position_base[] = {
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


template<unsigned int maxsymbols_p, unsigned int tablebits_p, bool allow_empty=false>
class HuffmanTable {
private:
	class LZXDStream *lzx;

public:
	static constexpr unsigned int HUFF_MAXBITS = 16;
	static constexpr unsigned int maxsymbols = maxsymbols_p;
	static constexpr unsigned int tablebits = tablebits_p;

	/**
	 * table of code lengths (in bits) for each symbol
	 */
	unsigned char len[maxsymbols_p + LZX_LENTABLE_SAFETY];

	/**
	 * table to fill up with decoded values for symbols, and pointers.
	 * symbols shorter than tablebits_p are decoded by direct lookup
	 * (that's the first (1<<tablebits_p) entries).
	 * longer symbols are decoded by following pointers to the appropriate
	 * leaf in the upper nsyms*2 fields.
	 */
	uint16_t table[(1 << tablebits_p) + (maxsymbols_p * 2)];

	/**
	 * the table could not be constructed and is empty (try_make_decode_table failed).
	 * this will lead to errors, should read_sym() be called.
	 */
	bool is_empty;

	HuffmanTable(class LZXDStream *lzx) : lzx{lzx}, is_empty{true} {}

	/**
	 * Decodes the next huffman symbol from the input bitstream,
	 * and returns it.
	 * Do not use this function on a table unless build_decode_table() succeeded.
	 */
	int read_sym();

	/**
	 * calls try_make_decode_table, and, if allow_empty is false,
	 * raises an exception on failure.
	 * builds a huffman lookup table from code lengths.
	 */
	void make_decode_table();

	/**
	 * reads in code lengths for symbols first to last in the given table.
	 * The code lengths are stored in their own special LZX way.
	 */
	void read_lengths(unsigned int first, unsigned int last);

private:
	/**
	 * This function was originally coded by David Tritscher.
	 * It builds a fast huffman decoding table from
	 * a canonical huffman code lengths table.
	 */
	bool try_make_decode_table();
};


class LZXDStream {
public:
	ssize_t          output_pos;        // number of output bytes

	unsigned char   *window;            // decoding window
	unsigned int     window_size;       // window size
	unsigned int     window_posn;       // decompression offset within window
	unsigned int     frame_posn;        // current frame offset within in window
	unsigned int     frame;             // the number of 32kb frames processed
	unsigned int     reset_interval;    // which frame do we reset the compressor?

	unsigned int     R0, R1, R2;        // for the LRU offset system
	unsigned int     block_length;      // uncompressed length of this LZX block
	unsigned int     block_remaining;   // uncompressed bytes still left to decode

	signed int       e8_magic;          // magic value that's used by the intel E8 transform.
	                                    // this field is read from the first 33 bits of the stream
	                                    // after reset, and called intel_filesize in libmspack.
	bool             header_read;       // indicates whether the e8_magic header has been read.

	unsigned char    block_type;        // type of the current block
	unsigned char    posn_slots;        // how many posn slots in stream?

	// IO buffering
	BitStream<4096> bits;

	// huffman code lenghts
	HuffmanTable<LZX_PRETREE_MAXSYMBOLS, LZX_PRETREE_TABLEBITS> htpre;
	HuffmanTable<LZX_MAINTREE_MAXSYMBOLS, LZX_MAINTREE_TABLEBITS> htmain;
	HuffmanTable<LZX_LENGTH_MAXSYMBOLS, LZX_LENGTH_TABLEBITS, true> htlength;
	HuffmanTable<LZX_ALIGNED_MAXSYMBOLS, LZX_ALIGNED_TABLEBITS> htaligned;

	/** See the doc for LZXDecompressor::LZXDecompressor in lzxd.h */
	LZXDStream(read_callback_t callback, unsigned int window_bits, unsigned int reset_interval);
	~LZXDStream();

	LZXDStream(const LZXDStream &other) = delete;
	LZXDStream(LZXDStream &&other) = delete;
	LZXDStream &operator =(const LZXDStream &other) = delete;
	LZXDStream &operator =(LZXDStream &&other) = delete;

	/** See the doc for LZXDecompressor::decompress_next_frame in lzxd.h */
	unsigned decompress_next_frame(unsigned char *output_buf);
private:
	/**
	 * Initializes the next block.
	 */
	void init_next_block();

	/**
	 * Decodes one symbol from the current verbatim block.
	 * Returns the number of bytes that were decoded.
	 * The bytes are written to the window, and window_posn is advanced.
	 */
	int decode_symbol_from_verbatim_block();

	/**
	 * Decodes one symbol from the current aligned block.
	 * Returns the number of bytes that were decoded.
	 * The bytes are written to the window, and window_posn is advanced.
	 */
	int decode_symbol_from_aligned_block();

	/**
	 * Reads the given amount of bytes from the current uncompressed block.
	 * Returns its argument (the number of bytes that were read).
	 * The bytes are written to the window, and window_posn is advanced.
	 */
	unsigned int read_data_from_uncompressed_block(unsigned int size);

	/**
	 * Postprocesses the a frame's decoded data with E8 decoding.
	 *
	 * Before compressing data, LZX pre-processes it by translating the four bytes
	 * following 0xE8-bytes; this optimizes intel x86 machine code.
	 *
	 * What the hell, Microsoft?
	 */
	void postprocess_intel_e8(unsigned char *output_buf, int frame_size);
public:
	void reset_state();
};


template<unsigned int maxsymbols_p, unsigned int tablebits_p, bool allow_empty>
int HuffmanTable<maxsymbols_p, tablebits_p, allow_empty>::read_sym() {
	lzx->bits.ensure_bits(HUFF_MAXBITS);
	uint16_t sym = table[lzx->bits.peek_bits(tablebits)];

	unsigned i = 1 << (sizeof(lzx->bits.bit_buffer) * 8 - tablebits);
	while (sym >= maxsymbols) {
		// huff_traverse
		if (unlikely((i >>= 1) == 0)) {
			throw Error(MSG(err) << "huff_error in huff_traverse");
		}
		sym = table[(sym << 1) | ((lzx->bits.bit_buffer & i) ? 1 : 0)];
	}

	lzx->bits.remove_bits(len[sym]);
	return sym;
}


template<unsigned int maxsymbols_p, unsigned int tablebits_p, bool allow_empty>
void HuffmanTable<maxsymbols_p, tablebits_p, allow_empty>::make_decode_table() {
	if (this->try_make_decode_table()) {
		this->is_empty = false;
	} else {
		if (allow_empty) {
			// allow an empty tree, but don't decode symbols with it

			// if any of the symbols has a greater-than-zero length, fail.
			for (unsigned int i = 0; i < maxsymbols; i++) {
				if (unlikely(this->len[i] > 0)) {
					throw Error(MSG(err) << "failed to build HuffmanTable<allow_empty=true>");
				}
			}
			this->is_empty = true;
		} else {
			throw Error(MSG(err) << "failed to build HuffmanTable<allow_empty=false>");
		}
	}
}


template<unsigned int maxsymbols_p, unsigned int tablebits_p, bool allow_empty>
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
			if ((pos += bit_mask) > table_mask) {
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
	return pos == table_mask;
}


template<unsigned int maxsymbols_p, unsigned int tablebits_p, bool allow_empty>
void HuffmanTable<maxsymbols_p, tablebits_p, allow_empty>::read_lengths(unsigned int first, unsigned int last) {
	// bit buffer and huffman symbol decode variables
	unsigned int x, y;
	int z;

	// read lengths for pretree (20 symbols, lengths stored in fixed 4 bits)
	for (x = 0; x < 20; x++) {
		lzx->htpre.len[x] = lzx->bits.read_bits(4);
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


LZXDStream::LZXDStream(read_callback_t read_callback,
                       unsigned int window_bits,
                       unsigned int reset_interval)
		:
		output_pos{0},
		window_size{(unsigned int) 1 << window_bits},
		window_posn{0},
		frame_posn{0},
		frame{0},
		reset_interval{reset_interval},
		bits{read_callback},
		htpre{this},
		htmain{this},
		htlength{this},
		htaligned{this} {

	// LZX supports window sizes of 2^15 (32 KiB) through 2^21 (2 MiB)
	if (window_bits < 15 || window_bits > 21) {
		throw Error(MSG(err) << "Bad requested window size: 2^" << window_bits << " bytes");
	}

	// allocate decompression window
	this->window = new unsigned char[window_size];

	// window bits:    15  16  17  18  19  20  21
	// position slots: 30  32  34  36  38  42  50
	this->posn_slots = 2 * window_bits;
	if (window_bits == 21) {
		this->posn_slots = 50;
	}
	if (window_bits == 20) {
		this->posn_slots = 42;
	}

	this->reset_state();
}


LZXDStream::~LZXDStream() {
	delete[] this->window;
}


void LZXDStream::reset_state() {
	this->R0 = 1;
	this->R1 = 1;
	this->R2 = 1;
	this->header_read = false;
	this->block_remaining = 0;
	this->block_type = LZX_BLOCKTYPE_INVALID;

	// initialise tables to 0 (because deltas will be applied to them)
	for (unsigned int i = 0; i < LZX_MAINTREE_MAXSYMBOLS; i++) {
		this->htmain.len[i] = 0;
	}

	for (unsigned int i = 0; i < LZX_LENGTH_MAXSYMBOLS; i++) {
		this->htlength.len[i] = 0;
	}
}


void LZXDStream::init_next_block() {
	// we might still be in bytstream mode from the previous block.
	this->bits.switch_to_bitstream_mode();

	// read block type (3 bits) and block length (24 bits)
	this->block_type = this->bits.read_bits(3);

	this->block_length = this->bits.read_bits(16) << 8;
	this->block_length |= this->bits.read_bits(8);
	this->block_remaining = this->block_length;

	// read individual block headers
	switch (this->block_type) {
	case LZX_BLOCKTYPE_ALIGNED:
		// read lengths of and build aligned huffman decoding tree
		for (int i = 0; i < 8; i++) {
			this->htaligned.len[i] = this->bits.read_bits(3);
		}
		this->htaligned.make_decode_table();

		// Falls through.
		// Rest of aligned header is same as verbatim
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
		this->bits.switch_to_bytestream_mode();

		// read 12 bytes of stored R0 / R1 / R2 values
		this->R0 = this->bits.read_4bytes_le();
		this->R1 = this->bits.read_4bytes_le();
		this->R2 = this->bits.read_4bytes_le();

		break;

	default:
		throw Error(MSG(err) << "decrunch: bad block type " << this->block_type);
	}
}


int LZXDStream::decode_symbol_from_verbatim_block() {
	int main_element = this->htmain.read_sym();
	if (main_element < (int) LZX_NUM_CHARS) {
		// literal: 0 to LZX_NUM_CHARS-1
		window[this->window_posn++] = main_element;
		return 1;
	}

	// match: LZX_NUM_CHARS + ((slot<<3) | length_header (3 bits))
	main_element -= LZX_NUM_CHARS;

	// get match length
	int match_length = main_element & LZX_NUM_PRIMARY_LENGTHS;
	if (match_length == LZX_NUM_PRIMARY_LENGTHS) {
		if (unlikely(this->htlength.is_empty)) {
			throw Error(MSG(err) << "decrunch: LENGTH symbol needed byt tree is empty");
		}
		int length_footer = this->htlength.read_sym();
		match_length += length_footer;
	}
	match_length += LZX_MIN_MATCH;

	// get match offset
	unsigned match_offset = (main_element >> 3);
	switch (match_offset) {
	case 0:
		match_offset = this->R0;
		break;
	case 1:
		match_offset = this->R1;
		this->R1 = this->R0;
		this->R0 = match_offset;
		break;
	case 2:
		match_offset = this->R2;
		this->R2 = this->R0;
		this->R0 = match_offset;
		break;
	case 3:
		match_offset = 1;
		this->R2 = this->R1;
		this->R1 = this->R0;
		this->R0 = match_offset;
		break;
	default:
		int extra = extra_bits[match_offset];
		int verbatim_bits = this->bits.read_bits(extra);
		match_offset = position_base[match_offset] - 2 + verbatim_bits;
		this->R2 = this->R1;
		this->R1 = this->R0;
		this->R0 = match_offset;
	}

	if (unlikely((this->window_posn + match_length) > this->window_size)) {
		throw Error(MSG(err) << "decrunch: match ran over window wrap");
	}

	// copy match
	unsigned char *rundest = &window[this->window_posn];
	int i = match_length;
	// does match offset wrap the window?
	if (match_offset > this->window_posn) {
		// j = length from match offset to end of window
		int j = match_offset - this->window_posn;
		if (unlikely(j > (int) this->window_size)) {
			throw Error(MSG(err) << "decrunch: match offset beyond window boundaries");
		}
		unsigned char *runsrc = &window[this->window_size - j];
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
		unsigned char *runsrc = rundest - match_offset;
		while (i-- > 0) {
			*rundest++ = *runsrc++;
		}
	}

	this->window_posn += match_length;
	return               match_length;
}


int LZXDStream::decode_symbol_from_aligned_block() {
	int main_element = this->htmain.read_sym();
	if (main_element < (int) LZX_NUM_CHARS) {
		// literal: 0 to LZX_NUM_CHARS - 1
		window[this->window_posn++] = main_element;
		return 1;
	}

	// match: LZX_NUM_CHARS + ((slot<<3) | length_header (3 bits))
	main_element -= LZX_NUM_CHARS;

	// get match length
	int match_length = main_element & LZX_NUM_PRIMARY_LENGTHS;
	if (match_length == LZX_NUM_PRIMARY_LENGTHS) {
		if (unlikely(this->htlength.is_empty)) {
			throw Error(MSG(err) << "decrunch: length symbol needed byt tree is empty");
		}
		int length_footer = this->htlength.read_sym();
		match_length += length_footer;
	}
	match_length += LZX_MIN_MATCH;

	// get match offset
	unsigned match_offset = (main_element >> 3);
	switch (match_offset) {
	case 0:
		match_offset = this->R0;
		break;
	case 1:
		match_offset = this->R1;
		this->R1 = this->R0;
		this->R0 = match_offset;
		break;
	case 2:
		match_offset = this->R2;
		this->R2 = this->R0;
		this->R0 = match_offset;
		break;
	default:
		int extra = extra_bits[match_offset];
		match_offset = position_base[match_offset] - 2;
		if (extra > 3) {
			// verbatim and aligned bits
			extra -= 3;
			int verbatim_bits = this->bits.read_bits(extra);
			match_offset += (verbatim_bits << 3);
			int aligned_bits = this->htaligned.read_sym();
			match_offset += aligned_bits;
		}
		else if (extra == 3) {
			// aligned bits only
			int aligned_bits = this->htaligned.read_sym();
			match_offset += aligned_bits;
		}
		else if (extra > 0) {
			// extra==1, extra==2
			// verbatim bits only
			int verbatim_bits = this->bits.read_bits(extra);
			match_offset += verbatim_bits;
		}
		else {
			// extra == 0
			// ??? not defined in LZX specification!
			match_offset = 1;
		}

		// update repeated offset LRU queue
		this->R2 = this->R1;
		this->R1 = this->R0;
		this->R0 = match_offset;
	}

	if (unlikely((this->window_posn + match_length) > this->window_size)) {
		throw Error(MSG(err) << "decrunch: match ran over window wrap");
	}

	// copy match
	unsigned char *rundest = &window[this->window_posn];
	int i = match_length;
	// does match offset wrap the window?
	if (match_offset > this->window_posn) {
		// j = length from match offset to end of window
		int j = match_offset - this->window_posn;
		if (unlikely(j > (int) this->window_size)) {
			throw Error(MSG(err) << "decrunch: match offset beyond window boundaries");
		}
		unsigned char *runsrc = &window[this->window_size - j];
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
		unsigned char *runsrc = rundest - match_offset;
		while (i-- > 0) {
			*rundest++ = *runsrc++;
		}
	}

	this->window_posn += match_length;
	return               match_length;
}


unsigned int LZXDStream::read_data_from_uncompressed_block(unsigned int size) {
	unsigned int remaining = size;

	while (remaining) {
		unsigned int amount = this->bits.read_bytes(&this->window[this->window_posn], remaining);
		this->window_posn += amount;
		remaining -= amount;
	}

	return size;
}


unsigned LZXDStream::decompress_next_frame(unsigned char *output_buf) {
	if (unlikely(this->bits.eof)) {
		return 0;
	}

	// have we reached the reset interval? (if there is one?)
	if (unlikely(this->reset_interval && ((this->frame % this->reset_interval) == 0))) {
		if (unlikely(this->block_remaining)) {
			throw Error(MSG(err) << this->block_remaining << " bytes remaining at reset interval");
		}

		// re-read the intel header and reset the huffman lengths
		reset_state();
	}

	// read header if necessary (after initializing or resetting the stream).
	if (unlikely(!this->header_read)) {
		// the first bit of the header indicates whether the e8_magic field is present.
		if (this->bits.read_bits(1)) {
			// read e8_magic (32 bits).
			this->e8_magic = this->bits.read_bits(16) << 16;
			this->e8_magic |= this->bits.read_bits(16);
		} else {
			// e8_magic is zero.
			this->e8_magic = 0;
		}

		this->header_read = true;
	}

	// counter that stores the amount of data that has been accumulated for this frame.
	unsigned int frame_size = 0;

	if (unlikely(this->frame_posn - this->window_posn)) {
		// Warning: untested code.
		// In theory, a symbol may have overshot the last frame's boundary.
		// If it did, the amount of data would be available in frame_size.

		throw Error(MSG(err) <<
			"untested code path: extra frame data available from last frame. " <<
			"frame size = " << this->frame_posn - this->window_posn);
	}

	// decode symbols until we have enough data for the frame.
	while (frame_size < LZX_FRAME_SIZE) {
		// initialise next block, if one is needed
		if (unlikely(this->block_remaining == 0)) {
			if (this->bits.eof) {
				// EOF of input stream was reached at a block boundary.
				// there are no more blocks. Return the frame as-is.
				break;
			}

			this->init_next_block();
		}

		unsigned int symbol_size;

		switch (this->block_type) {
		case LZX_BLOCKTYPE_VERBATIM:
			symbol_size = this->decode_symbol_from_verbatim_block();
			break;
		case LZX_BLOCKTYPE_ALIGNED:
			symbol_size = this->decode_symbol_from_aligned_block();
			break;
		case LZX_BLOCKTYPE_UNCOMPRESSED:
			symbol_size = this->read_data_from_uncompressed_block(std::min(this->block_remaining, LZX_FRAME_SIZE - frame_size));
			break;
		default:
			throw Error(MSG(err) << "this->blocktype neither verbatim nor aligned");
		}

		if (unlikely(symbol_size > this->block_remaining)) {
			// we overshot the block boundary.
			throw Error(MSG(err) << "decrunch: overrun went past end of block by " << symbol_size - this->block_remaining);
		}

		this->block_remaining -= symbol_size;
		frame_size += symbol_size;
	}

	if (unlikely(frame_size > LZX_FRAME_SIZE)) {
		// Warning: untested code.
		// In theory, a symbol may overshoot a frame boundary. If it does, the data
		// will get re-used in the next frame, but we have to limit frame_size to LZX_FRAME_SIZE.

		throw Error(MSG(err) << "untested code path: frame_size > LZX_FRAME_SIZE");

		// TODO: unreachable code
		// frame_size = LZX_FRAME_SIZE;
	}

	// streams don't extend over frame boundaries
	if (unlikely(this->window_posn != this->frame_posn + frame_size)) {
		throw Error(MSG(err) << "decrunch: decode beyond output frame limits");
	}

	// copy frame data to the output buffer
	memcpy(output_buf, &this->window[this->frame_posn], frame_size);

	// intel e8-postprocess the frame data
	this->postprocess_intel_e8(output_buf, frame_size);

	// frame boundary; re-align the bitstream, if it's currently in bitstream mode.
	this->bits.align_if_in_bitstream_mode();

	// advance frame start position
	this->frame_posn += frame_size;

	// wrap window / frame position pointers
	if (this->window_posn == this->window_size) {
		window_posn = 0;
	}
	if (this->frame_posn == this->window_size) {
		this->frame_posn = 0;
	}

	this->output_pos += frame_size;
	this->frame++;

	return frame_size;
}


void LZXDStream::postprocess_intel_e8(unsigned char *buf, int frame_size) {
	if (this->e8_magic == 0) {
		// this lzx stream is e8-processing-free
		return;
	}

	if (this->frame >= 32768) {
		// in frames >= 32768 (>= 1GiB of plain data) there's no e8 processing (WTF WHY?)
		return;
	}

	// search the block for occurances of '0xe8'
	// the last 10 bytes of the frame are not e8-handled, because reasons.
	for (int pos = 0; pos < frame_size - 10; pos++) {
		bool is_e8 = (buf[pos] == 0xe8);

		if (!is_e8) {
			continue;
		}

		// we've found an E8 sequence.
		// the next 4 bytes shall be translated.

		int file_pos = pos + this->output_pos;

		// read the next 4 bytes as little endian
		int32_t abs_offset =
			(buf[pos + 1] <<  0) |
			(buf[pos + 2] <<  8) |
			(buf[pos + 3] << 16) |
			(buf[pos + 4] << 24);

		if (abs_offset >= -file_pos && abs_offset < this->e8_magic) {
			int32_t rel_offset;
			if (abs_offset >= 0) {
				rel_offset = abs_offset - file_pos;
			} else {
				rel_offset = abs_offset + this->e8_magic;
			}

			// write the next 4 bytes as little endian
			buf[pos + 1] = (unsigned char) (rel_offset >>  0);
			buf[pos + 2] = (unsigned char) (rel_offset >>  8);
			buf[pos + 3] = (unsigned char) (rel_offset >> 16);
			buf[pos + 4] = (unsigned char) (rel_offset >> 24);
		}

		// skip the next four bytes (they are what we just translated).
		pos += 4;
	}
}


LZXDecompressor::LZXDecompressor(read_callback_t read_callback,
                                 unsigned int window_bits,
                                 unsigned int reset_interval)
	:
	stream{new LZXDStream{read_callback, window_bits, reset_interval}} {}


LZXDecompressor::~LZXDecompressor() {
	delete stream;
}


unsigned LZXDecompressor::decompress_next_frame(unsigned char *output_buf) {
	return this->stream->decompress_next_frame(output_buf);
}


}}} // openage::util::compress
