#ifndef _ENGINE_CONSOLE_BUF_
#define _ENGINE_CONSOLE_BUF_

#include <stdint.h>
#include <stdlib.h>

#include <vector>

#include "../coord/term.h"
#include "../util/unicode.h"

namespace engine {
namespace console {

/**
 * the length of the escape sequence buffer, and thus the maximum length
 * of any escape sequence that will be successfully processed.
 *
 * longer escape sequences will be ignored.
 */
using chrcol_t = uint8_t;
using chrflags_t = uint16_t;

constexpr chrflags_t CHR_BOLD = (1 << 0);
constexpr chrflags_t CHR_FAINT = (1 << 1);
constexpr chrflags_t CHR_ITALIC = (1 << 2);
constexpr chrflags_t CHR_BLINKING = (1 << 3);
constexpr chrflags_t CHR_BLINKINGFAST = (1 << 4);
constexpr chrflags_t CHR_UNDERLINED = (1 << 5);
constexpr chrflags_t CHR_STRUCKOUT = (1 << 6);
constexpr chrflags_t CHR_FRAKTUR = (1 << 7);
constexpr chrflags_t CHR_NEGATIVE = (1 << 8);
constexpr chrflags_t CHR_INVISIBLE = (1 << 9);
constexpr chrflags_t CHR_FRAMED = (1 << 10);
constexpr chrflags_t CHR_ENCIRCLED = (1 << 11); //because framing stuff is so 20th century
constexpr chrflags_t CHR_OVERLINED = (1 << 12); //whoever knows that this exists
constexpr chrflags_t CHR_RIGHTLINED = (1 << 13); //whoever wants that
constexpr chrflags_t CHR_LEFTLINED = (1 << 14); //whoever needs that
constexpr chrflags_t CHR_STRESS_IDEOGRAM = (1 << 15); //whatever that is

/**
 * a single character in the buffer
 */
struct buf_char {
	/**
	 * unicode codepoint of the character
	 */
	int cp;
	/**
	 * (256-col) color of the character
	 */
	chrcol_t fgcol;
	/**
	 * (256-col) background color
	 */
	chrcol_t bgcol;
	/**
	 * flags of the character
	 */
	chrflags_t flags;

	bool operator ==(const buf_char &other) {
		return
		 (this->cp == other.cp) &&
		 (this->fgcol == other.fgcol) &&
		 (this->bgcol == other.bgcol) &&
		 (this->flags == other.flags);
	}

	bool operator !=(const buf_char &other) {
		return not (*this == other);
	}
};

constexpr buf_char BUF_CHAR_DEFAULT {0x20, 248, 0, 0};

using linetype_t = uint8_t;

/**
 * the line has not been written yet (or cleared in between)
 */
constexpr linetype_t LINE_EMPTY = 0;
/**
 * the line has some contents
 */
constexpr linetype_t LINE_REGULAR = 1;
/**
 * the line has been written to until it auto-wrapped;
 * its continuation can be found in the next line
 *
 * this value is required for re-calculating the buffer on resize
 */
constexpr linetype_t LINE_WRAPPED = 2;

/**
 * metadata for a single line in the buffer
 */
struct buf_line {
	linetype_t type;
};

constexpr buf_line BUF_LINE_DEFAULT {LINE_EMPTY};

class Buf {
public:
	//following this line are all terminal buffer related variables

	/**
	 * the complete (2-dimensional) terminal output buffer content
	 *
	 * contains the screen buffer (where the cursor may move),
	 * and the scrollback buffer (which is invisible while not scrolled back)
	 *
	 * thus, its size is always
	 *   screen buffer size + scrollback buffer size
	 *   (dims.x * dims.y + dims.x * scrollback_lines)
	 *
	 * on resize, this buffer is completely re-created.
	 *
	 * the first entry of data is not neccesarily the first character of the
	 * screen or scrollback buffer; see screen_chrdata.
	 */
	buf_char *chrdata;

	/**
	 * stores the (array) size of data, for performance and convenience
	 * reasons.
	 *
	 * always has the value
	 *   dims.x * (dims.y + scrollback_lines)
	 */
	size_t chrdata_size;

	/**
	 * stores the end of the region allocated for data, for performance
	 * and convenience reasons.
	 * points to the first memory region outside the data buffer.
	 *
	 * always has the value
	 *   data + chrdata_size
	 */
	buf_char *chrdata_end;

	/**
	 * points to the first character that belongs to the screen buffer,
	 * inside the data buffer.
	 * this pointer is changed when the terminal buffer advances one line.
	 *
	 * note that screen_chrdata may NOT be directly indexed, as *screen_chrdata[k]
	 * might be >= chrdata_end. in this case, the correct memory location will
	 * be *screen_chrdata[k - chrdata_size].
	 *
	 * *screen_chrdata[0] always is the correct memory location of the screen
	 * buffer top left corner.
	 *
	 * *screen_chrdata[dims.x * dims.y] is the memory location of the scrollback
	 * buffer top left corner, but it might be neccesary to subtract
	 * chrdata_size to get an actually valid memory location.
	 */
	buf_char *screen_chrdata;

	/**
	 * similar to how data holds information about all characters,
	 * linedata holds information about all lines.
	 * currently, the only held information is whether the line has been started
	 * by wrapping an existing line (and thus is not a 'real' new line)
	 */
	buf_line *linedata;

	/**
	 * see chrdata_size
	 */
	size_t linedata_size;

	/**
	 * see chrdata_end
	 */
	buf_line *linedata_end;

	/**
	 * as an analog to screen_chrdata, this points to the entry in linedata that
	 * holds metadata about the top line of the screen buffer.
	 */
	buf_line *screen_linedata;

	//following this line are all terminal size related variables

	/**
	 * minimum screen buffer width
	 */
	coord::term_t min_width;

	/**
	 * screen buffer dimensions (in characters)
	 */
	coord::term dims;

	/**
	 * scrollback buffer height;
	 * scrollback buffer width is identical to screen buffer width.
	 */
	coord::term_t scrollback_lines;

	//following this line are all cursor state related variables

	/**
	 * cursor position
	 */
	coord::term cursorpos;

	/**
	 * saved cursor position
	 */
	coord::term saved_cursorpos;

	/**
	 * true if the cursor is visible
	 */
	bool cursor_visible;

	/**
	 * true if cursor is in the last column while whole line is filled
	 * this is very special.
	 * this allows CR after fully-filled lines to return to the same line,
	 * instead of being in the next line.
	 */
	bool cursor_special_lastcol;

	//following this line are misc variables

	/**
	 * true if we are currently reading an escape sequence
	 */
	bool escaped;

	/**
	 * current escape sequence
	 */
	std::vector<int> escape_sequence;

	/**
	 * true if an bell ('\a') has been received, but not cleared
	 */
	bool bell;

	/**
	 * window title
	 */
	std::vector<int> title;

	/**
	 * surrently selected formatting (colors, flags).
	 * all printed chars, as well as all buffer clearing,
	 * will use this until it is changed by an SGR escape sequence.
	 *
	 * the codepoint is the codepoint used for filling empty chars
	 * (e.g. 0x20 ('space'))
	 */
	buf_char current_char_fmt;

	/**
	 * how far it's currently possible to scroll back.
	 * this value steadily increases when the buffer advances.
	 *
	 * if this is 0, all lines outside the screen buffer are empty.
	 *
	 * must be >= 0 and <= scrollback_lines
	 */
	coord::term_t scrollback_possible;

	/**
	 * how many lines the buffer is currently scrolled back.
	 * if   0, the screen buffer is wholly rendered.
	 *         if the terminal is being advanced, this stays 0.
	 *         this makes the terminal follow newly printed text.
	 * if > 0, the bottom lines of the screen buffer are not rendered,
	 *         and the bottom lines of the scrollback buffer are rendered
	 *         instead.
	 *         if screen buffer advances, this is increased accordingly
	 *         (unless it would become >= scrollback_lines).
	 *         this causes the currently scrolled-to position to remain
	 *         scrolled-to even when new text is printed.
	 *
	 * must be >= 0 and <= scrollback_possible.
	 */
	coord::term_t scrollback_pos;

	/**
	 * resets the buffer to its constructor defaults
	 */
	void reset();

	/**
	 * write a byte string to the buffer
	 *
	 * the string is assumed to be UTF-8 encoded.
	 *
	 * if len >= 0, it describes the length of the string.
	 * otherwise, the string is assumed to be null-terminated.
	 */
	void write(const char *c, ssize_t len = -1);

	/**
	 * write a single byte to the buffer.
	 *
	 * assumed to be UTF-8 encoded.
	 */
	void write(char c);

	Buf(coord::term dims, coord::term_t scrollback_lines, coord::term_t min_width);
	~Buf();

	/*
	 * we don't want this object to be copyable
	 */
	Buf& operator=(const Buf &) = delete;
	Buf(const Buf &) = delete;

	/**
	 * resizes the screen buffer
	 */
	void resize(coord::term new_dims);

	/**
	 * scrolls up or down by the given number of lines
	 *
	 * lines
	 *   amount of lines to scroll up - may be negative
	 *   if the upper or lower limit is reached, this is capped.
	 */
	void scroll(coord::term_t lines);

	/**
	 * prints the buffer content to stdout
	 *
	 * clear
	 *   if true, the whole content of stdout is cleared
	 */
	void to_stdout(bool clear);

private:
	/**
	 * utf-8 state-machine that reads input bytes
	 * and outputs unicode codepoints
	 */
	util::utf8_decoder streamdecoder;

	/**
	 * processes one unicode codepoint
	 * internally called by write(char) after feeding the
	 * char into streamdecoder
	 */
	void process_codepoint(int cp);

	/**
	 * prints one unicode codepoint
	 * interally called by process_codepoint(int cp) after sorting out escape sequences, and by some escape sequences such as CSI @
	 */
	void print_codepoint(int cp);

	/**
	 * aborts the current escape sequence
	 * (e.g. because it contained an illegal
	 * character, or is not implemented)
	 */
	void escape_sequence_aborted();

	/**
	 * called when a escape sequence has been
	 * successfully processed, clears the
	 * sequence. either this or abort_escape_sequence
	 * is called for each escape sequence.
	 */
	void escape_sequence_processed();

	/**
	 * processes the CSI escape sequence currently stored in
	 * escape_sequence.
	 * invalid sequences are ignored.
	 */
	void process_csi_escape_sequence();

	/**
	 * processes a single-codepoint escape sequence.
	 * invalid codepoints are ignored.
	 */
	void process_escaped_cp(int cp);

	/**
	 * processes a text escape sequence
	 * that is one of OSC, DCS, APC or PM
	 */
	void process_text_escape_sequence();

	/**
	 * sets graphics rendition parameters; called by
	 * process_escape_sequence
	 *
	 * params:
	 *   numerical params list from escape sequence
	 */
	void process_sgr_code(const std::vector<int> &params);

	/**
	 * advances the buffer by the specified number of lines.
	 *
	 * the top lines are moved to the scrollback buffer.
	 * the top lines of the scrollback buffer are discarded.
	 * the bottom lines of the buffer are empty after this.
	 * the cursor position is not changed by this.
	 *
	 * linecount:
	 *   the number of lines to advance the buffer;
	 *   defaults to one.
	 *   may be > buffer height.
	 */
	void advance(unsigned linecount = 1);

	/**
	 * clears the screen character and line buffers.
	 * lines are cleared iff all of their characters are cleared.
	 *
	 * if start >= end, nothing happens.
	 *
	 * inputs are not sanitized.
	 *
	 * start
	 *   screen buffer coordinates of first character to be deleted
	 *   negative values of y indicate scrollback buffer
	 *   -scrollback_lines <= y < dims.y
	 *   0 <= x < dims.x
	 * end
	 *   screen buffer coordinates of first character not to be deleted
	 *   negative values of y indicate scrollback buffer
	 *   -scrollback_lines <= y < dims.y
	 *   0 <= x < dims.x
	 *   {x, y} = {0, dims.y} is allowed.
	 * clear_end
	 *   if true, end is incremented by 1. this affects all constraints.
	 */
	void clear(coord::term start, coord::term end, bool clear_end = false);

	/**
	 * clears a range of the data buffer.
	 * end is the first character that is not cleared.
	 * end and start both must be valid pointers within data.
	 */
	void chrdata_clear(buf_char *start, buf_char *end);

	/**
	 * clears a range of the linedata buffer.
	 * end is the first line that is not cleared.
	 * end and start both must be valid pointers within data.
	 */
	void linedata_clear(buf_line *start, buf_line *end);

	/**
	 * returns a valid pointer to the character info for the character
	 * designated by pos
	 *
	 * pos
	 *   screen buffer coordinates
	 *   negative values of y indicate scrollback buffer
	 *   -scrollback_lines <= pos.y <= dims.y
	 *   0 <= pos.x < dims.x
	 *   if y == dims.y, the return ptr is not guaranteed to be valid.
	 */
	buf_char *chrdataptr(coord::term pos);

	/**
	 * returns a valid pointer to the line info for the line designated
	 * by lineno
	 *
	 * lineno
	 *   number of line in screen buffer
	 *   negative numbers indicate scrollback buffer
	 *   -scrollback_lines <= lineno <= dims.y
	 *   if lineno == dims.y, the return ptr is not guaranteed to be valid.
	 */
	buf_line *linedataptr(coord::term_t lineno);
};

} //namespace console
} //namespace engine

#endif //_ENGINE_CONSOLE_BUF_
