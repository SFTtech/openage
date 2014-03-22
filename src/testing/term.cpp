#include "term.h"

#include <unistd.h>
#include <termios.h>
#include <pty.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include "../engine/console/buf.h"
#include "../engine/log.h"

using namespace engine;
using namespace engine::console;
using namespace engine::coord;

namespace testing {

struct termios old_tio;
void setstdincanon() {
	if (isatty(STDIN_FILENO)) {
		//get the terminal settings for stdin
		tcgetattr (STDIN_FILENO, &old_tio);
		//backup old settings
		struct termios new_tio = old_tio;
		//disable buffered i/o (canonical mode) and local echo
		new_tio.c_lflag &= (~ICANON & ~ECHO & ~ISIG);
		//set the settings
		tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
	}
}

void restorestdin() {
	if (isatty(STDIN_FILENO)) {
		tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
	}
}

bool tests::term0(int /*unused*/, char ** /*unused*/) {

	Buf buf{{80, 25}, 1337, 80};
	buf.write("Hello, brave new console world!\n\n\n\n");
	buf.write("stuff, lol.\n\n");
	buf.write("\x1b[1mbold stuff, lol.\x1b[m\n\n");
	buf.write("\x1b[5;31;1mred bold blinking stuff, lol, ... also, this text seems to be quite long, maybe even wider than the terminal width. i wonder what could \x1b[7mhappen.\x1b[m\n\n");
	for (int i = 0; i < 18; i++) {
		buf.write("asdf\n");
	}
	buf.scroll(100);
	buf.to_stdout(true);
	return true;
}

int dup_as_nonblocking(int fd) {
	int newfd = dup(fd);
	int flags = fcntl(newfd, F_GETFL, 0);
	fcntl(newfd, F_SETFL, flags | O_NONBLOCK);
	return newfd;
}

int max(int a, int b) {
	return (a > b) ? a : b;
}

bool tests::term1(int /*unused*/, char ** /*unused*/) {
	Buf buf{{80, 25}, 1337, 80};
	struct winsize ws;
	ws.ws_col = buf.dims.x;
	ws.ws_row = buf.dims.y;
	ws.ws_xpixel = buf.dims.x * 8;
	ws.ws_ypixel = buf.dims.y * 13;
	int amaster;
	switch (forkpty(&amaster, nullptr, nullptr, &ws)) {
	case -1:
		log::err("fork() failed");
		return false;
		break;
	case 0:
		//we are the child, spawn a shell
		{
		const char *shell = getenv("SHELL");
		if (shell == nullptr) {
			shell = "/bin/sh";
		}
		execl(shell, shell, nullptr);
		log::err("execl(\"%s\", \"%s\", nullptr) failed", shell, shell);
		}
		return false;
		break;
	default:
		//we are the parent
		break;
	}

	//hide cursor
	printf("\x1b[?25l");
	//set stdin canon
	setstdincanon();

	constexpr int rdbuf_size = 4096;
	char rdbuf[rdbuf_size];

	int nonblocking_stdin = dup_as_nonblocking(0);
	int nonblocking_amaster = dup_as_nonblocking(amaster);

	int nfds = max(nonblocking_stdin, nonblocking_amaster) + 1;

	bool loop = true;

	buf.to_stdout(true);

	while (loop) {
		fd_set rfds;
		struct timeval tv;

		/* Watch stdin (fd 0) to see when it has input. */
		FD_ZERO(&rfds);
		FD_SET(nonblocking_stdin, &rfds);
		FD_SET(nonblocking_amaster, &rfds);

		tv.tv_sec = 0;
		tv.tv_usec = 1000000 / 60;

		switch (select(nfds, &rfds, NULL, NULL, &tv)) {
		case -1:
			//error
			break;
		case 0:
			//timeout
			break;
		default:
			//success
			if (FD_ISSET(nonblocking_stdin, &rfds)) {
				ssize_t retval = read(nonblocking_stdin, rdbuf, rdbuf_size);
				switch (retval) {
				case -1:
					//error... probably EWOULDBLOCK. ignore.
					break;
				case 0:
					//EOF on stdin... huh... well... that was unexpected... TODO
					break;
				default:
					write(amaster, rdbuf, retval);
				}
			}
			if (FD_ISSET(nonblocking_amaster, &rfds)) {
				ssize_t retval = read(nonblocking_amaster, rdbuf, rdbuf_size);
				switch (retval) {
				case -1:
					switch(errno) {
					case EIO:
						loop = false;
						break;
					default:
						//probably EWOULDBLOCK. ignore.
						break;
					}
					break;
				case 0:
					//EOF on amaster
					loop = false;
					break;
				default:
					for(int i = 0; i < retval; i++) {
						buf.write(rdbuf[i]);
					}
				}
			}
			break;
		}

		if (tv.tv_usec == 0) {
			buf.to_stdout(false);

			tv.tv_usec = 1000000 / 60;
		}
	}

	//show cursor
	printf("\x1b[?25h");
	//restore stdin
	restorestdin();

	close(nonblocking_stdin);
	close(nonblocking_amaster);
	close(amaster);

	return true;
}

} //namespace testing
