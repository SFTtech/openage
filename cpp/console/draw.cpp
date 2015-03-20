// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "draw.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../crossplatform/opengl.h"
#include "../crossplatform/timing.h"
#include <FTGL/ftgl.h>

#include <unistd.h>

#include "console.h"

namespace openage {
namespace console {
namespace draw {

void to_opengl(Console *console) {
	coord::camhud topleft = {
		console->bottomleft.x,
		console->bottomleft.y + console->charsize.y * console->buf.dims.y
	};
	coord::camhud chartopleft;
	coord::pixel_t ascender = console->font.internal_font->Ascender();

	int64_t monotime = timing::get_monotonic_time();

	bool fastblinking_visible = (monotime % 600000000 < 300000000);
	bool slowblinking_visible = (monotime % 300000000 < 150000000);

	for (coord::term_t x = 0; x < console->buf.dims.x; x++) {
		chartopleft.x = topleft.x + console->charsize.x * x;

		for (coord::term_t y = 0; y < console->buf.dims.y; y++) {
			chartopleft.y = topleft.y - console->charsize.y * y;
			buf_char p = *(console->buf.chrdataptr({x, y - console->buf.scrollback_pos}));

			int fgcolid, bgcolid;

			bool cursor_visible_at_current_pos = (
				console->buf.cursorpos == coord::term{x, y - console->buf.scrollback_pos}
			);

			cursor_visible_at_current_pos &= console->buf.cursor_visible;

			if ((p.flags & CHR_NEGATIVE) xor cursor_visible_at_current_pos) {
				bgcolid = p.fgcol;
				fgcolid = p.bgcol;
			} else {
				bgcolid = p.bgcol;
				fgcolid = p.fgcol;
			}

			if ((p.flags & CHR_INVISIBLE)
			    or (p.flags & CHR_BLINKING and not slowblinking_visible)
			    or (p.flags & CHR_BLINKINGFAST and not fastblinking_visible)) {
				fgcolid = bgcolid;
			}

			console->termcolors[bgcolid].use(0.8);

			glBegin(GL_QUADS);
			{
				glVertex3f(chartopleft.x, chartopleft.y, 0);
				glVertex3f(chartopleft.x, chartopleft.y - console->charsize.y, 0);
				glVertex3f(chartopleft.x + console->charsize.x, chartopleft.y - console->charsize.y, 0);
				glVertex3f(chartopleft.x + console->charsize.x, chartopleft.y, 0);
			}
			glEnd();

			console->termcolors[fgcolid].use(1);

			char utf8buf[5];
			if (util::utf8_encode(p.cp, utf8buf) == 0) {
				//unrepresentable character (question mark in black rhombus)
				console->font.render_static(chartopleft.x, chartopleft.y - ascender, "\uFFFD");
			} else {
				console->font.render_static(chartopleft.x, chartopleft.y - ascender, utf8buf);
			}
		}
	}
}

void to_terminal(Buf *buf, util::FD *fd, bool clear) {
	//move cursor, draw top left corner
	fd->puts("\x1b[H\u250c");
	if (clear) {
		fd->puts("\x1b[J");
	}
	//draw top line, including title
	for (coord::term_t x = 0; x < buf->dims.x; x++) {
		if (x >= 3 && (x - 3) < (int) buf->title.size()) {
			fd->putcp(buf->title[x - 3]);
		} else if (x == 2) {
			fd->putbyte('[');
		} else if (x - 3 == (int) buf->title.size()) {
			fd->putbyte(']');
		} else {
			fd->puts("\u2500");
		}
	}
	//draw top right corner
	fd->puts("\u2510\n");
	//calculate pos/size of scrollbar
	//scrollbar_top is the first line that has the scrollbar displayed
	//scrollbar_bottom is the first line that doesn't have the scrollbar displayed
	coord::term_t lines_total      = buf->scrollback_possible + buf->dims.y;
	coord::term_t pos_total        = buf->scrollback_possible - buf->scrollback_pos;
	coord::term_t scrollbar_top    = (buf->dims.y * pos_total) / lines_total;
	coord::term_t scrollbar_bottom = (buf->dims.y * (pos_total + buf->dims.y)) / lines_total;
	if (scrollbar_bottom == scrollbar_top) {
		if (scrollbar_bottom < buf->dims.y) {
			scrollbar_bottom++;
		} else {
			scrollbar_top--;
		}
	}

	//print lines -scrollback_pos to dims.y - scrollback_pos - 1
	for (coord::term_t y = 0; y < buf->dims.y; y++) {
		//draw left line
		fd->puts("\u2502");
		//draw chars of this line
		for (coord::term_t x = 0; x < buf->dims.x; x++) {
			buf_char p = *(buf->chrdataptr({x, y - buf->scrollback_pos}));
			if (p.cp < 32) {
				p.cp = '?';
			}
			fd->printf("\x1b[38;5;%dm\x1b[48;5;%dm", p.fgcol, p.bgcol);
			if (p.flags & CHR_BOLD) {
				fd->puts("\x1b[1m");
			}
			if (p.flags & CHR_BLINKING) {
				fd->puts("\x1b[5m");
			}
			bool cursor_visible_at_current_pos = buf->cursorpos == coord::term{x, y - buf->scrollback_pos};
			cursor_visible_at_current_pos &= buf->cursor_visible;
			if ((p.flags & CHR_NEGATIVE) xor cursor_visible_at_current_pos) {
				//print char negative
				fd->puts("\x1b[7m");
			}
			fd->putcp(p.cp);
			fd->puts("\x1b[m");
		}
		//draw right line
		if (y >= scrollbar_top and y < scrollbar_bottom) {
			//draw scrollbar on this part of right line
			fd->puts("\u2503");
		} else {
			fd->puts("\u2502");
		}
		fd->putbyte('\n');
	}
	//draw bottom left corner
	fd->puts("\u2514");
	//draw bottom line
	for (coord::term_t x = 0; x < buf->dims.x; x++) {
		fd->puts("\u2500");
	}
	//draw bottom right corner
	fd->puts("\u2518\n");
}

} //namespace draw
} //namespace console
} //namespace openage
