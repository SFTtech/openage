// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#include <vector>
#include <string>

#ifdef _MSC_VER
#define STDOUT_FILENO 1
#else
#include <unistd.h>
#endif
#include "../util/fds.h"
#include "../util/pty.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include "../log/log.h"
#include "../error/error.h"

#include "buf.h"
#include "console.h"
#include "draw.h"

namespace openage {
namespace console {
namespace tests {


// TODO: move to util
int max(int a, int b) {
	return (a > b) ? a : b;
}


// TODO: test for console coordinates and resizing


void render() {
	console::Buf buf{{80, 25}, 1337, 80};

	buf.write("Hello, brave new console world!\n\n\n\n");
	buf.write("stuff, lol.\n\n");
	buf.write("\x1b[1mbold stuff, lol.\x1b[m\n\n");
	buf.write("\x1b[5;31;1mred bold blinking stuff, lol, ... also, this text seems to be quite long, maybe even wider than the terminal width. i wonder what could \x1b[7mhappen.\x1b[m\n\n");
	for (int i = 0; i < 18; i++) {
		buf.write("rofl\n");
	}
	buf.scroll(100);
	util::FD outfd{STDOUT_FILENO};
	console::draw::to_terminal(&buf, &outfd, true);
}


void interactive() {
	#ifndef _WIN32

	console::Buf buf{{80, 25}, 1337, 80};
	struct winsize ws;

	ws.ws_col = buf.dims.x;
	ws.ws_row = buf.dims.y;
	ws.ws_xpixel = buf.dims.x * 8;
	ws.ws_ypixel = buf.dims.y * 13;

	int amaster;
	switch (forkpty(&amaster, nullptr, nullptr, &ws)) {
	case -1:
		throw Error(MSG(err) << "fork() failed: " << strerror(errno));
	case 0: {
		// we are the child, spawn a shell
		const char *shell = getenv("SHELL");
		if (shell == nullptr) {
			shell = "/bin/sh";
		}
		execl(shell, shell, nullptr);
		throw Error(MSG(err) << "execl(\"" << shell << "\", \"" << shell << "\", nullptr) failed: " << strerror(errno));
	}
	default:
		//we are the parent
		break;
	}

	util::FD ptyout{amaster};            // for writing to the slave
	util::FD ptyin{&ptyout, true};       // for reading the slave's output
	util::FD termout{STDOUT_FILENO};     // for writing to the terminal
	util::FD termin{STDIN_FILENO, true}; // for reading the user input

	// hide cursor
	termout.puts("\x1b[?25l");
	// set canon input mode
	termin.setinputmodecanon();
	// set amaster to auto-close
	ptyout.close_on_destroy = true;

	constexpr int rdbuf_size = 4096;
	char rdbuf[rdbuf_size];

	int nfds = max(termin.fd, ptyin.fd) + 1;

	bool loop = true;

	console::draw::to_terminal(&buf, &termout, true);

	while (loop) {
		fd_set rfds;
		struct timeval tv;

		// Watch stdin (fd 0) to see when it has input.
		FD_ZERO(&rfds);
		FD_SET(ptyin.fd, &rfds);
		FD_SET(termin.fd, &rfds);

		tv.tv_sec = 0;
		tv.tv_usec = 1000000 / 60;

		switch (select(nfds, &rfds, NULL, NULL, &tv)) {
		case -1:
			// error
			break;
		case 0:
			// timeout
			break;
		default:
			// success
			if (FD_ISSET(termin.fd, &rfds)) {
				ssize_t retval = read(termin.fd, rdbuf, rdbuf_size);
				switch (retval) {
				case -1:
					// error... probably EWOULDBLOCK. ignore.
					break;
				case 0:
					// EOF on stdin... huh... well... that was unexpected... TODO
					break;
				default:
					if (ptyout.write(rdbuf, retval) != retval) {
						// for some reason, we couldn't write all input to
						// a master.
						loop = false;
					}
					break;
				}
			}
			if (FD_ISSET(ptyin.fd, &rfds)) {
				ssize_t retval = read(ptyin.fd, rdbuf, rdbuf_size);
				switch (retval) {
				case -1:
					switch(errno) {
					case EIO:
						loop = false;
						break;
					default:
						// probably EWOULDBLOCK. ignore.
						break;
					}
					break;
				case 0:
					// EOF on amaster
					loop = false;
					break;
				default:
					for (int i = 0; i < retval; i++) {
						buf.write(rdbuf[i]);
					}
				}
			}
			break;
		}

		if (tv.tv_usec == 0) {
			console::draw::to_terminal(&buf, &termout, false);

			tv.tv_usec = 1000000 / 60;
		}
	}

	// show cursor
	termout.puts("\x1b[?25h");

	#endif /* _WIN32 */
}


}}} // namespace openage::console::tests
