#include "buf.h"

#include "../util/unicode.h"

#include "stdio.h"

namespace engine {
namespace console {

using namespace coord;

Buf::Buf(term dims, term_t scrollback_lines, term_t min_width) {
	//init all member variables
	this->min_width = min_width;
	if (dims.x < this->min_width) {
		dims.x = this->min_width;
	}
	if (dims.y < 1) {
		dims.y = 1;
	}
	this->dims = dims;
	this->scrollback_lines = scrollback_lines;

	this->linedata_size = this->dims.y + this->scrollback_lines;
	this->linedata = new buf_line[this->linedata_size];
	this->linedata_end = this->linedata + this->linedata_size;

	this->chrdata_size = dims.x * linedata_size;
	this->chrdata = new buf_char[this->chrdata_size];
	this->chrdata_end = this->chrdata + this->chrdata_size;

	this->screen_chrdata = this->chrdata;
	this->screen_linedata = this->linedata;

	this->cursorpos = {0, 0};
	this->saved_cursorpos = {0, 0};
	this->cursor_visible = true;

	this->escaped = false;
	this->bell = false;
	this->current_char_fmt = BUF_CHAR_DEFAULT;

	this->scrollback_possible = 0;
	this->scrollback_pos = 0;

	//fully clear data and linedata
	this->clear({0, (term_t) -this->scrollback_lines}, {0, this->dims.y});
}

Buf::~Buf() {
	//free dynamically allocated members
	delete[] this->linedata;
	delete[] this->chrdata;
}

/*
 * this class merely exists as a helper for the next function,
 * Buf::resize(), and is thus defined and documented here in the implementation
 * cpp file.
 *
 * its purpose is to provide a new buffer, to where the old buffer may be
 * copied easily, avoiding code duplication.
 */
class NewBuf {
public:
	term dims;
	term_t scrollback_lines;

	buf_line *linedata;
	size_t linedata_size;
	buf_line *linedata_end;
	buf_line *linedata_ptr;

	buf_char *chrdata;
	size_t chrdata_size;
	buf_char *chrdata_end;
	buf_char *chrdata_ptr;

	term_t screen_buf_start;
	size_t screen_buf_size;
	term_t scrollback_buf_start;
	size_t scrollback_buf_size;

	bool current_line_is_screen_buf;
	term current_pos;

	NewBuf(term dims, term_t scrollback_lines) {
		this->dims = dims;
		this->scrollback_lines = scrollback_lines;

		this->linedata_size = this->dims.y + this->scrollback_lines;
		this->linedata = new buf_line[this->linedata_size];
		this->linedata_end = this->linedata + this->linedata_size;

		this->chrdata_size = this->linedata_size * this->dims.x;
		this->chrdata = new buf_char[this->chrdata_size];
		this->chrdata_end = this->chrdata + this->chrdata_size;

		this->screen_buf_start = 0;
		this->screen_buf_size = 0;
		this->scrollback_buf_start = 0;
		this->scrollback_buf_size = 0;

		this->current_line_is_screen_buf = false;
		this->current_pos = {0, 0};
	}

	~NewBuf() {
		delete this->linedata;
		delete this->chrdata;
	}

	void apply_to_buf(Buf *b) {
		b->dims = this->dims;

		delete b->linedata;
		b->linedata = this->linedata;
		this->linedata = nullptr;
		b->linedata_size = this->linedata_size;
		b->linedata_end = this->linedata_end;

		delete b->chrdata;
		b->chrdata = this->chrdata;
		this->chrdata = nullptr;
		b->chrdata_size = this->chrdata_size;
		b->chrdata_end = this->chrdata_end;

		//TODO set the following members of b:
		//screen_chrdata, screen_linedata
		//cursorpos, saved_cursorpos,
		//scrollback_possible, scrollback_pos
		//TODO call b->clear() with correct
		//screen buffer part as arguments
	}

	void move_ptrs_to_next_line() {
		//all three if clauses should have identical results...
		this->current_pos.y++;
		if (this->current_pos.y >= this->dims.y) {
			this->current_pos.y = 0;
		}
		this->linedata_ptr++;
		if (this->linedata_ptr >= this->linedata_end) {
			this->linedata_ptr = this->linedata;
		}
		this->chrdata_ptr += this->dims.x;
		if (this->chrdata_ptr >= this->chrdata_end) {
			this->chrdata_ptr = this->chrdata;
		}
		*(this->linedata_ptr) = BUF_LINE_DEFAULT;
		this->current_pos.x = 0;
	}

	/**
	 * starts a new line (as by newline character)
	 */
	void new_line(bool part_of_screen_buf) {
		//clear the remaining chars of current line
		for (; this->current_pos.x < this->dims.x; this->current_pos.x++) {
			this->chrdata_ptr[this->current_pos.x] = BUF_CHAR_DEFAULT;
		}

		move_ptrs_to_next_line();

		this->current_line_is_screen_buf = part_of_screen_buf;
	}

	/**
	 * writes a char
	 */
	void new_chr(buf_char c) {
		this->chrdata_ptr[this->current_pos.x++] = c;

		if (c != BUF_CHAR_DEFAULT) {
			this->linedata_ptr->type = LINE_REGULAR;

			if (this->current_line_is_screen_buf) {
				//TODO write down somewhere the fact that this
				//line constitutes a part of the screen buffer
			} else {
				//TODO write down somewhere the fact that this
				//line constitutes a part of the scrollback buffer
			}
		}

		if (this->current_pos.x == this->dims.x) {
			this->linedata_ptr->type = LINE_WRAPPED;
			move_ptrs_to_next_line();
		}
	}
};

//TODO unfinished implementation
void Buf::resize(term new_dims) {
	term old_dims = this->dims;

	if (new_dims.x < this->min_width) {
		new_dims.x = this->min_width;
	}
	if (new_dims.y < 1) {
		new_dims.y = 1;
	}

	//allocate new buffers
	size_t new_linedata_size = new_dims.y + this->scrollback_lines;
	buf_line *new_linedata = new buf_line[new_linedata_size];
	buf_line *new_linedata_end = new_linedata + new_linedata_size;
	size_t new_chrdata_size = new_dims.x * new_linedata_size;
	buf_char *new_chrdata = new buf_char[new_chrdata_size];
	buf_char *new_chrdata_end = new_chrdata + new_chrdata_size;

	//working positions in new buffers: start at beginning of buffer
	buf_line *new_linedata_pos = new_linedata;
	buf_char *new_chrdata_pos = new_chrdata;
	//working positions in old buffers: start with oldest line of
	//scrollback buffer
	buf_line *old_linedata_scrollbackstart = linedataptr(-scrollback_lines);
	buf_char *old_chrdata_scrollbackstart = chrdataptr({0, (term_t) -scrollback_lines});
	buf_line *old_linedata_pos = old_linedata_scrollbackstart;
	buf_char *old_chrdata_pos = old_chrdata_scrollbackstart;

	//copy line by line, considering the value stored in
	//old_linedata_pos->auto_wrapped.
	//start with the first line of the scrollback buffer, and stop with the
	//last line of the screen buffer.
	//do not copy empty characters, unless they are followed by filled
	//characters in the same line, or a continued wrapped line.
	//store in term_t variables the locations in the new buffer of
	//  the first/last line of the old scrollback buffer
	//  the first/last line of the old screen buffer
	//these variables need to take into account events where upmost
	//lines of a buffer are overwritten by new lines.
	//commented out for now //TODO

	//term_t scrollback_buf_start;
	//term_t scrollback_buf_end;
	//term_t screen_buf_start;
	//term_t screen_buf_end;

	//count the number of empty chars that we've found in this line,
	//and we may or may not still have to copy.
	size_t empty_chars = 0;
	while(old_linedata_pos != old_linedata_scrollbackstart) {
		//should never be >, always ==
		//also, both checks should always yield identical results
		//(TODO do an ASSERT to assure this?)
		if (old_linedata_pos >= this->linedata_end) {
			old_linedata_pos = this->linedata;
		}
		if (old_chrdata_pos >= this->chrdata_end) {
			old_chrdata_pos = this->chrdata;
		}
		//should never be >, always ==
		//also, both checks should always yield identical results
		//(TODO do an ASSERT to assure this?)
		if (new_linedata_pos >= new_linedata_end) {
			new_linedata_pos = new_linedata;
		}
		if (new_chrdata_pos >= new_chrdata_end) {
			new_chrdata_pos = new_chrdata;
		}

		for(term_t x = 0; x < old_dims.x; x++) {
			if (*old_chrdata_pos == BUF_CHAR_DEFAULT) {
				empty_chars++;
			} else {
				while (empty_chars > 0) {
					//TODO write the empty chars
				}
				//TODO write the char
			}

			old_chrdata_pos++;
		}
	}

	//TODO
	//depending on the variables defined in the previous section,
	//decide which line is the first line of the screen buffer
	//int new_chrbuf, and which parts of new_chrbuf must be cleared.

	//TODO
	//copy

	this->dims = new_dims;
}

void Buf::write(const char *c, ssize_t len) {
	if (len >= 0) {
		for(; len > 0; len--) {
			this->write(*c);
			c++;
		}
	} else {
		for(; *c; c++) {
			this->write(*c);
		}
	}
}

void Buf::scroll(term_t lines) {
	if (lines < 0) {
		//scroll down
		lines = -lines;

		if ((term_t) this->scrollback_pos < lines) {
			this->scrollback_pos = 0;
		} else {
			this->scrollback_pos -= lines;
		}
	} else {
		//scroll up
		this->scrollback_pos += lines;
		if (this->scrollback_pos > this->scrollback_possible) {
			this->scrollback_pos = this->scrollback_possible;
		}
	}
}

void Buf::advance(unsigned linecount) {
	//sanitize linecount
	if (linecount == 0) {
		return;
	}
	if ((term_t) linecount > this->dims.y + this->scrollback_lines) {
		linecount = this->dims.y + this->scrollback_lines;
	}

	//update scrollback_possible
	this->scrollback_possible += linecount;
	if (this->scrollback_possible > (term_t) this->scrollback_lines) {
		this->scrollback_possible = this->scrollback_lines;
	}

	//update scrollback position, to remain at the currently scrolled-to
	//position
	if (this->scrollback_pos > 0) {
		this->scrollback_pos += linecount;
		if (this->scrollback_pos > this->scrollback_possible) {
			//seems like we're already on top of the scrollback buffer
			this->scrollback_pos = this->scrollback_possible;
		}
	}

	//clear the new lines. that's scrollback_buffer[0:linecount]
	this->clear({0, (term_t) -this->scrollback_lines}, {0, (term_t) ((term_t) linecount - (term_t) this->scrollback_lines)});

	//move the screen buffer by updating the screen_chrdata pointer
	this->screen_chrdata += linecount * this->dims.x;
	if (this->screen_chrdata >= this->chrdata_end) {
		this->screen_chrdata -= this->chrdata_size;
	}

	//also update the screen_linedata pointer
	this->screen_linedata += linecount;
	if (this->screen_linedata >= this->linedata_end) {
		this->screen_linedata -= this->linedata_size;
	}
}

void Buf::write(char c) {
	if (!this->streamdecoder.feed(c)) {
		//an error has been detected in the input stream
		this->write_codepoint(0xFFFD);
	};

	if(this->streamdecoder.remaining == 0 && this->streamdecoder.out >= 0) {
		this->write_codepoint(streamdecoder.out);
	}
}

void Buf::write_codepoint(int cp) {
	//depending on the terminal state, we might do different things here.
	if (this->escaped) {
		this->escape_sequence.push_back(cp);

		//we need special treatment if this is the first character of the
		//escape sequence (we expect a '[').
		if (this->escape_sequence.size() == 1) {
			if (cp == '[') {
				//everything is in order.
				return;
			} else {
				//nothing is in order.
				//this is an illegal escape sequence.


				//abortabortabort
				this->escaped = false;
				this->escape_sequence.clear();
				return;
			}
		}


		if (cp >= 0x40 && cp < 0x7f) {
			//this char terminates the escape sequence
			this->escaped = false;

			this->process_escape_sequence();

			this->escape_sequence.clear();
			return;
		} else if (cp >= 0x20 && cp < 0x40) {
			//this char is appended to the escape sequence
			return;
		} else {
			//this char is not allowed. abortabortabort
			this->escaped = false;
			this->escape_sequence.clear();
			return;
		}
	} else {
		switch (cp) {
		//control characters
		case 0x00: //NUL: ignore
		case 0x01: //SOH: ignore
		case 0x02: //STX: ignore
		case 0x03: //ETX: ignore
		case 0x04: //EOT: ignore
		case 0x05: //ENQ: ignore (empty response)
		case 0x06: //ACK: ignore
			break;
		case 0x07: //BEL: set bell flag
			this->bell = true;
			break;
		case 0x08: // BS: backspace
			//move cursor 1 left.
			//if cursor pos is 0, move to end of previous line
			//if already at (0,0), move to (0,0)
			this->cursorpos.x -= 1;
			if (this->cursorpos.x < 0) {
				this->cursorpos.x += this->dims.x;
				this->cursorpos.y -= 1;
				if (this->cursorpos.y < 0) {
					this->cursorpos = {0, 0};
				}
			}
			break;
		case 0x09: // HT: horizontal tab
			//move cursor to next multiple of 8
			//at least move by 1, at most by 8
			//if next multiple of 8 is greater than terminal width,
			//move to end of this line
			//never move to next line
			this->cursorpos.x = ((this->cursorpos.x + 8) / 8) * 8;
			if (this->cursorpos.x >= this->dims.x) {
				this->cursorpos.x = this->dims.x - 1;
			}
			break;
		case 0x0a: // LF: linefeed
		case 0x0b: // VT: vertical tab
		case 0x0c: // FF: form feed
			//all these do the same: move to beginning of next line
			this->cursorpos.x = 0;

			if (this->cursorpos.y == this->dims.y - 1) {
				this->advance(1);
			} else {
				this->cursorpos.y += 1;
			}
			break;
		case 0x0d: // CR: ignore
		case 0x0e: // SO: ignore
		case 0x0f: // SI: ignore
		case 0x10: //DLE: ignore
		case 0x11: //DC1: ignore
		case 0x12: //DC2: ignore
		case 0x13: //DC3: ignore
		case 0x14: //DC4: ignore
		case 0x15: //NAK: ignore
		case 0x16: //SYN: ignore
		case 0x17: //ETB: ignore
		case 0x18: //CAN: ignore
		case 0x19: // EM: ignore
		case 0x1a: //SUB: ignore
			break;
		case 0x1b: //ESC: escape sequence start
			this->escaped = true;
			break;
		case 0x1c: // FS: ignore
		case 0x1d: // GS: ignore
		case 0x1e: // RS: ignore
		case 0x1f: // US: ignore
		case 0x7f: //DEL: ignore
			break;
		default:   //regular, printable character
			//set char at current cursor pos
			buf_char *ptr = this->chrdataptr(this->cursorpos);
			*ptr = this->current_char_fmt;
			ptr->cp = cp;
			buf_line *lineptr = this->linedataptr(this->cursorpos.y);
			//store the fact that this line has been written to
			if (lineptr->type == LINE_EMPTY) {
				lineptr->type = LINE_REGULAR;
			}
			//advance cursor to the right
			this->cursorpos.x++;
			if(this->cursorpos.x == this->dims.x) {
				//store the fact that this line was auto-wrapped
				//and will continue in the next line
				lineptr->type = LINE_WRAPPED;

				this->cursorpos.x = 0;
				if (this->cursorpos.y == this->dims.y - 1) {
					this->advance(1);
				} else {
					this->cursorpos.y += 1;
				}
			}
			break;
		}
	}
}

void Buf::process_escape_sequence() {
	printf("process escape sequence ");
	for(int c: this->escape_sequence) {
		if (c < 0x20 || c > 128) {
			c = '?';
		}
		printf("%c", c);
	}
	printf("\n"); //ASDF

	size_t len = this->escape_sequence.size();
	int type = this->escape_sequence[len - 1];
	bool starts_with_questionmark = false;
	std::vector<int> params;
	bool in_param = false;
	int currentparam;

	for(size_t pos = 1; pos < len - 1; pos++) {
		int cp = this->escape_sequence[pos];
		if (cp == '?' && pos == 1) {
			starts_with_questionmark = true;
		} else if (cp >= '0' && cp <= '9') {
			if (!in_param) {
				in_param = true;
				currentparam = 0;
			}

			currentparam = 10 * currentparam + (cp - '0');
		} else if (cp == ';') {
			if (!in_param) {
				//this ';' did not have a number before it
				//(ommited)
				currentparam = -1;
			}

			params.push_back(currentparam);
			in_param = false;
		} else {
			//unexpected character, we want a nice
			//semicolon-separated list of decimal numbers.
			//abortabortabort
			return;
		}
	}

	if (in_param) {
		//there's no semicolon at the end, finish the param
		params.push_back(currentparam);
	} else {
		//there's a semicolon at the end, or we had no params
		//at all. add param with default value.
		params.push_back(-1);
	}

	//default values
	int defaultval;
	unsigned minparamcount;
	switch (type) {
	case 'H':
	case 'f':
		minparamcount = 2;
		defaultval = 1;
		break;
	case 'J':
	case 'K':
	case 'm':
		minparamcount = 1;
		defaultval = 0;
		break;
	case 's':
	case 'u':
		minparamcount = 0;
		defaultval = 0;
		break;
	default:
		minparamcount = 1;
		defaultval = 1;
		break;
	}

	//apply default value requirements
	while (params.size() < minparamcount) {
		params.push_back(-1);
	}
	for (int &p: params) {
		if (p == -1) {
			p = defaultval;
		}
	}

	//execute the escape sequence
	switch (type) {
	case 'A': //CUU: move cursor up
		this->cursorpos.y -= params[0];
		if (this->cursorpos.y < 0) {
			this->cursorpos.y = 0;
		}
		break;
	case 'B': //CUD: move cursor down
		this->cursorpos.y += params[0];
		if (this->cursorpos.y >= this->dims.y) {
			this->cursorpos.y = this->dims.y - 1;
		}
		break;
	case 'C': //CUF: move cursor to the right
		this->cursorpos.x += params[0];
		if (this->cursorpos.x >= this->dims.x) {
			this->cursorpos.x = this->dims.x - 1;
		}
		break;
	case 'D': //CUB: move cursor to the left
		this->cursorpos.x -= params[0];
		if (this->cursorpos.x < 0) {
			this->cursorpos.x = 0;
		}
		break;
	case 'E': //CNL: move cursor down and to beginning of line
		this->cursorpos.x = 0;
		this->cursorpos.y += params[0];
		if (this->cursorpos.y >= this->dims.y) {
			this->cursorpos.y = this->dims.y - 1;
		}
		break;
	case 'F': //CPL: move cursor up and to beginning of line
		this->cursorpos.x = 0;
		this->cursorpos.y -= params[0];
		if (this->cursorpos.y < 0) {
			this->cursorpos.y = 0;
		}
		break;
	case 'G': //CHA: set cursorpos.x
		this->cursorpos.x = params[0] - 1;
		if (this->cursorpos.x < 0) {
			this->cursorpos.x = 0;
		}
		if (this->cursorpos.x >= this->dims.x) {
			this->cursorpos.x = this->dims.x - 1;
		}
		break;
	case 'f': //HVP: set cursorpos
		//fall through
	case 'H': //CUP: set cursorpos
		this->cursorpos.y = params[0];
		this->cursorpos.x = params[1];
		if (this->cursorpos.x < 0) {
			this->cursorpos.x = 0;
		}
		if (this->cursorpos.x >= this->dims.x) {
			this->cursorpos.x = this->dims.x - 1;
		}
		if (this->cursorpos.y < 0) {
			this->cursorpos.y = 0;
		}
		if (this->cursorpos.y >= this->dims.y) {
			this->cursorpos.y = this->dims.y - 1;
		}
		break;
	case 'J': // ED: erase display
		switch (params[0]) {
		case 0: //clear screen buffer from cursor to end
			this->clear(this->cursorpos, {0, this->dims.y});
			break;
		case 1: //clear screen buffer from beginning to cursor
			this->clear({0, 0}, this->cursorpos, true);
			break;
		case 2: //clear screen buffer
			this->clear({0, 0}, {0, this->dims.y});
			break;
		default://unknown/unimplemented parameter
			break;
		}
		break;
	case 'K': // EL
		switch (params[0]) {
		case 0: //clear current line from cursor to end
			this->clear(this->cursorpos, {0, (term_t) (this->cursorpos.y + 1)});
			break;
		case 1: //clear current line from beginning to cursor
			this->clear({0, this->cursorpos.y}, this->cursorpos, true);
			break;
		case 2: //clear current line
			this->clear({0, this->cursorpos.y}, {0, (term_t) (this->cursorpos.y + 1)});
			break;
		default://unknown/unimplemented parameter
			break;
		}
		break;
	case 'm': //SGR
		this->sgr(params);
		break;
	case 's': //SCP
		this->saved_cursorpos = this->cursorpos;
		break;
	case 'u': //RCP
		this->cursorpos = this->saved_cursorpos;
		break;
	case 'l': //set mode
		if (starts_with_questionmark) {
			switch(params[0]) {
			case 25: //cursor invisible
				this->cursor_visible = false;
				break;
			case 1049: //switch to alternate screen
				//TBI
				//idea: use last dims.y lines of scrollback
				//buffer to save regular screen data
				break;
			default: //unknown/unimplemented parameter
				break;
			}
		} else {
			switch(params[0]) {
			default: //unknown/unimplemented parameter
				break;
			}
			break;
		}
		break;
	case 'h': //reset mode
		if (starts_with_questionmark) {
			switch(params[0]) {
			case 25: //cursor visible
				this->cursor_visible = true;
				break;
			case 1049: //restore regular screen
				//TBI
				break;
			default: //unknown/unimplemented parameter
				break;
			}
		} else {
			switch(params[0]) {
			default: //unknown/unimplemented parameter
				break;
			}
			break;
		}
		break;
	case 'S': // SU
	case 'T': // SD
	case 'n': //DSR
		//not implemented; fall through.
	default:
		//not implemented (or nonexisting)
		//abortabortabort
		return;
	}
}

void Buf::sgr(const std::vector<int> &params) {
	for(size_t i = 0; i < params.size(); i++) {
		int p = params[i];
		switch (p) {
		case 0: //reset
			this->current_char_fmt = BUF_CHAR_DEFAULT;
			break;
		case 1: //bold
			this->current_char_fmt.flags |= CHR_BOLD;
			this->current_char_fmt.flags &= ~CHR_FAINT;
			break;
		case 2: //faint
			this->current_char_fmt.flags |= CHR_FAINT;
			this->current_char_fmt.flags &= ~CHR_BOLD;
			break;
		case 3: //italic
			this->current_char_fmt.flags |= CHR_ITALIC;
			this->current_char_fmt.flags &= ~CHR_FRAKTUR;
			break;
		case 4: //underline
			this->current_char_fmt.flags |= CHR_UNDERLINED;
			break;
		case 5: //blink slowly
			this->current_char_fmt.flags |= CHR_BLINKING;
			this->current_char_fmt.flags &= ~CHR_BLINKINGFAST;
			break;
		case 6: //blink fast
			this->current_char_fmt.flags |= CHR_BLINKINGFAST;
			this->current_char_fmt.flags &= ~CHR_BLINKING;
			break;
		case 7: //negative
			this->current_char_fmt.flags |= CHR_NEGATIVE;
			break;
		case 8: //invisible
			this->current_char_fmt.flags |= CHR_INVISIBLE;
			break;
		case 9: //struck out
			this->current_char_fmt.flags |= CHR_STRUCKOUT;
			break;
		//cases 10-19: font selectors. not implemented.
		case 20: //fraktur
			this->current_char_fmt.flags |= CHR_FRAKTUR;
			this->current_char_fmt.flags &= ~CHR_ITALIC;
			break;
		case 21: //bold off
			this->current_char_fmt.flags &= ~CHR_BOLD;
			break;
		case 22: //bold and faint off
			this->current_char_fmt.flags &= ~CHR_BOLD;
			this->current_char_fmt.flags &= ~CHR_FAINT;
			break;
		case 23: //italic and fraktur off
			this->current_char_fmt.flags &= ~CHR_ITALIC;
			this->current_char_fmt.flags &= ~CHR_FRAKTUR;
			break;
		case 24: //underline off
			this->current_char_fmt.flags &= ~CHR_UNDERLINED;
			break;
		case 25: //blinking and blinkingfast off
			this->current_char_fmt.flags &= ~CHR_BLINKING;
			this->current_char_fmt.flags &= ~CHR_BLINKINGFAST;
			break;
		//case 26: not yet standardized
		case 27: //negative off
			this->current_char_fmt.flags &= ~CHR_NEGATIVE;
			break;
		case 28: //invisible off
			this->current_char_fmt.flags &= ~CHR_INVISIBLE;
			break;
		case 29: //struck out off
			this->current_char_fmt.flags &= ~CHR_STRUCKOUT;
			break;
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37: //foreground color (8 colors)
			this->current_char_fmt.fgcol = (p - 30);
			break;
		case 38: //foreground color (256 colors)
			i += 2;
			if (i >= params.size() || params[i] < 0 || params[i] >= 256) {
				//invalid 256-color SGR code.
				//abortabortabort
				return;
			}
			this->current_char_fmt.fgcol = params[i];
			break;
		case 39: //reset foreground color
			this->current_char_fmt.fgcol = BUF_CHAR_DEFAULT.fgcol;
			break;
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47: //background color (8 colors)
			this->current_char_fmt.bgcol = (p - 40);
			break;
		case 48: //background color (256 colors)
			i += 2;
			if (i >= params.size() || params[i] < 0 || params[i] >= 256) {
				//invalid 256-color SGR code.
				//abortabortabort
				return;
			}
			this->current_char_fmt.bgcol = params[i];
			break;
		case 49: //reset background color
			this->current_char_fmt.bgcol = BUF_CHAR_DEFAULT.bgcol;
			break;
		//case 50: not yet standardized
		case 51: //framed
			this->current_char_fmt.flags |= CHR_FRAMED;
			this->current_char_fmt.flags &= ~CHR_ENCIRCLED;
			break;
		case 52: //encircled
			this->current_char_fmt.flags |= CHR_ENCIRCLED;
			this->current_char_fmt.flags &= ~CHR_FRAMED;
			break;
		case 53: //overlined
			this->current_char_fmt.flags |= CHR_OVERLINED;
			break;
		case 54: //framed and encircled off
			this->current_char_fmt.flags &= ~CHR_FRAMED;
			this->current_char_fmt.flags &= ~CHR_ENCIRCLED;
			break;
		case 55: //overlined off
			this->current_char_fmt.flags &= ~CHR_OVERLINED;
			break;
		//cases 56-59: not yet standardized
		case 60: //right side line
			this->current_char_fmt.flags |= CHR_RIGHTLINED;
			break;
		//case 61: double right side line. not implemented.
		case 62: //left side line
			this->current_char_fmt.flags |= CHR_LEFTLINED;
			break;
		//case 63: double left side line. not implemented.
		case 64: //stress ideogram
			this->current_char_fmt.flags |= CHR_STRESS_IDEOGRAM;
			break;
		case 65: //disables effects 60-64
			this->current_char_fmt.flags &= ~CHR_RIGHTLINED;
			this->current_char_fmt.flags &= ~CHR_LEFTLINED;
			break;
		//cases above 65 are not standardized.
		default:
			//not implemented or not defined. abortabortabort
			return;
		}
	}
}

void Buf::clear(term start, term end, bool clear_end) {
	//apply clear_end
	if (clear_end) {
		end.x++;
		if (end.x > this->dims.x) {
			end.x = 0;
			end.y++;
		}
	}

	if (start.y > end.y or (start.y == end.y and start.x >= end.x)) {
		return;
	}

	//clear char info
	chrdata_clear(chrdataptr(start), chrdataptr(end));

	//calculate lines to clear
	//a line is cleared iff all of its characters are cleared
	term_t line_start = start.y;
	if (start.x > 0) {
		line_start++;
	}
	term_t line_end = end.y;

	if (line_start >= line_end) {
		return;
	}

	//clear line info
	linedata_clear(linedataptr(line_start), linedataptr(line_end));
}

void Buf::chrdata_clear(buf_char *start, buf_char *end) {
	if (start < end) {
		for(; start < end; start++) {
			*start = this->current_char_fmt;
		}
	} else {
		for(buf_char *c = start; c < this->chrdata_end; c++) {
			*c = this->current_char_fmt;
		}
		for(buf_char *c = this->chrdata; c < end; c++) {
			*c = this->current_char_fmt;
		}
	}
}

void Buf::linedata_clear(buf_line *start, buf_line *end) {
	if (start < end) {
		for(; start < end; start++) {
			*start = BUF_LINE_DEFAULT;
		}
	} else {
		for(buf_line *l = start; l < this->linedata_end; l++) {
			*l = BUF_LINE_DEFAULT;
		}
		for(buf_line *l = this->linedata; l < end; l++) {
			*l = BUF_LINE_DEFAULT;
		}
	}
}

buf_char *Buf::chrdataptr(term pos) {
	buf_char *result = this->screen_chrdata + pos.x + pos.y * this->dims.x;
	if (result < this->chrdata) {
		result += this->chrdata_size;
	} else if (result >= this->chrdata_end) {
		result -= this->chrdata_size;
	}
	return result;
}

buf_line *Buf::linedataptr(term_t lineno) {
	buf_line *result = this->screen_linedata + lineno;
	if (result < this->linedata) {
		result += this->linedata_size;
	}
	if (result > this->linedata_end) {
		result -= this->linedata_size;
	}
	return result;
}

#include <stdio.h>
void Buf::to_stdout(bool clear) {
	if (clear) {
		//clear stdout
		printf("\x1b[2J");
	}
	//move cursor, draw top left corner
	printf("\x1b[H\u250c");
	//draw top line
	for (term_t x = 0; x < this->dims.x; x++) {
		printf("\u2500");
	}
	//draw top right corner
	printf("\u2510\n");
	//calculate pos/size of scrollbar
	//scrollbar_top is the first line that has the scrollbar displayed
	//scrollbar_bottom is the first line that doesn't have the scrollbar displayed
	term_t lines_total = this->scrollback_possible + this->dims.y;
	term_t pos_total = this->scrollback_possible - this->scrollback_pos;
	term_t scrollbar_top = (this->dims.y * pos_total) / lines_total;
	term_t scrollbar_bottom = (this->dims.y * (pos_total + this->dims.y)) / lines_total;
	if (scrollbar_bottom == scrollbar_top) {
		if (scrollbar_bottom < this->dims.y) {
			scrollbar_bottom++;
		} else {
			scrollbar_top--;
		}
	}

	//print lines -scrollback_pos to dims.y - scrollback_pos - 1
	for (term_t y = 0; y < this->dims.y; y++) {
		//draw left line
		printf("\u2502");
		//draw chars of this line
		for (term_t x = 0; x < this->dims.x; x++) {
			buf_char p = *(this->chrdataptr({x, y - this->scrollback_pos}));
			if (p.cp < 32) {
				p.cp = '?';
			}
			printf("\x1b[38;5;%dm\x1b[48;5;%dm", p.fgcol, p.bgcol);
			if (p.flags & CHR_BOLD) {
				printf("\x1b[1m");
			}
			if (p.flags & CHR_BLINKING) {
				printf("\x1b[5m");
			}
			if ((p.flags & CHR_NEGATIVE) xor (this->cursorpos == term{x, y - this->scrollback_pos})) {
				printf("\x1b[7m");
			}
			char utf8buf[5];
			if (util::utf8_encode(p.cp, utf8buf) == 0) {
				utf8buf[0] = '?';
				utf8buf[1] = '\0';
			}
			printf("%s", utf8buf);
			printf("\x1b[m");
		}
		//draw right line
		if (y >= scrollbar_top and y < scrollbar_bottom) {
			//draw scrollbar on this part of right line
			printf("\u2503");
		} else {
			printf("\u2502");
		}
		printf("\n");
	}
	//draw bottom left corner
	printf("\u2514");
	//draw bottom line
	for (term_t x = 0; x < this->dims.x; x++) {
		printf("\u2500");
	}
	//draw bottom right corner
	printf("\u2518\n");
}

} //namespace console
} //namespace engine
