#include "draw.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <FTGL/ftgl.h>

#include <unistd.h>

using namespace engine::coord;

namespace engine {
namespace console {
namespace draw {

void to_opengl(Buf *buf, Font *font, coord::camhud bottomleft, coord::camhud charsize) {
	coord::camhud topleft = {bottomleft.x, bottomleft.y + charsize.y * buf->dims.y};
	coord::camhud chartopleft;
	coord::pixel_t ascender = font->internal_font->Ascender();

	for (term_t x = 0; x < buf->dims.x; x++) {
		chartopleft.x = topleft.x + charsize.x * x;

		for (term_t y = 0; y < buf->dims.y; y++) {
			chartopleft.y = topleft.y - charsize.y * y;
			buf_char p = *(buf->chrdataptr({x, y - buf->scrollback_pos}));

			glColor4f(0, 0, 0, 0.8);
			glBegin(GL_QUADS);
			{
				glVertex3f(chartopleft.x, chartopleft.y, 0);
				glVertex3f(chartopleft.x, chartopleft.y - charsize.y, 0);
				glVertex3f(chartopleft.x + charsize.x, chartopleft.y - charsize.y, 0);
				glVertex3f(chartopleft.x + charsize.x, chartopleft.y, 0);
			}
			glEnd();

			glColor4f(1, 1, 0, 1);
			char utf8buf[5];
			if (util::utf8_encode(p.cp, utf8buf) == 0) {
				//unrepresentable character (question mark in black rhombus)
				font->render_static(chartopleft.x, chartopleft.y - ascender, "\uFFFD");
			} else {
				font->render_static(chartopleft.x, chartopleft.y - ascender, utf8buf);
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
	//draw top line
	for (term_t x = 0; x < buf->dims.x; x++) {
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
	term_t lines_total = buf->scrollback_possible + buf->dims.y;
	term_t pos_total = buf->scrollback_possible - buf->scrollback_pos;
	term_t scrollbar_top = (buf->dims.y * pos_total) / lines_total;
	term_t scrollbar_bottom = (buf->dims.y * (pos_total + buf->dims.y)) / lines_total;
	if (scrollbar_bottom == scrollbar_top) {
		if (scrollbar_bottom < buf->dims.y) {
			scrollbar_bottom++;
		} else {
			scrollbar_top--;
		}
	}

	//print lines -scrollback_pos to dims.y - scrollback_pos - 1
	for (term_t y = 0; y < buf->dims.y; y++) {
		//draw left line
		fd->puts("\u2502");
		//draw chars of this line
		for (term_t x = 0; x < buf->dims.x; x++) {
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
			bool cursor_visible_at_current_pos = buf->cursorpos == term{x, y - buf->scrollback_pos};
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
	for (term_t x = 0; x < buf->dims.x; x++) {
		fd->puts("\u2500");
	}
	//draw bottom right corner
	fd->puts("\u2518\n");
}

} //namespace draw
} //namespace console
} //namespace engine
