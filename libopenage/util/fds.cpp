// Copyright 2014-2019 the openage authors. See copying.md for legal info.

#include "fds.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <fcntl.h>
#include "pty.h"
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include "unicode.h"

namespace openage::util {

FD::FD(int fd) {
	this->fd = fd;
}

FD::FD(int fd, bool set_nonblocking) {
	this->fd = ::dup(fd);
	this->close_on_destroy = true;

	if (set_nonblocking) {
		#ifndef _WIN32
		int flags = ::fcntl(this->fd, F_GETFL, 0);
		::fcntl(this->fd, F_SETFL, flags | O_NONBLOCK);
		#endif
	}
}

FD::FD(FD *fd, bool set_nonblocking) :
	FD{fd->fd, set_nonblocking} {
}

FD::~FD() {
	if (this->restore_input_mode_on_destroy) {
		this->restoreinputmode();
	}

	if (this->close_on_destroy) {
		::close(this->fd);
	}
}

int FD::write(const char *buf, size_t bytes) {
	return ::write(this->fd, buf, bytes);
}

int FD::puts(const char *str) {
	return this->write(str, strlen(str));
}

int FD::putbyte(char c) {
	return this->write(&c, 1);
}

int FD::putcp(int cp) {
	char utf8buf[5];
	if (util::utf8_encode(cp, utf8buf) == 0) {
		//unrepresentable character (question mark in black rhombus)
		return this->puts("\uFFFD");
	} else {
		return this->puts(utf8buf);
	}
}

int FD::printf(const char *format, ...) {
	const unsigned buf_size = 16;
	char *buf = static_cast<char*>(malloc(sizeof(char) * buf_size));
	if (!buf) {
		return -1;
	}

	va_list vl;

	//first, try to vsnprintf to a buffer of length 16
	va_start(vl, format);
	unsigned len = vsnprintf(buf, buf_size, format, vl);
	va_end(vl);

	//if that wasn't enough, allocate more memory and try again
	if (len >= buf_size) {
		char *oldbuf = buf;
		buf = static_cast<char*>(realloc(oldbuf, sizeof(char) * (len + 1)));
		if (!buf) {
			free(oldbuf);
			return -1;
		}

		va_start(vl, format);
		vsnprintf(buf, len + 1, format, vl);
		va_end(vl);
	}

	//output buf to the socket
	int result = this->puts(buf);

	//free the buffer
	free(buf);

	return result;
}

void FD::setinputmodecanon() {
	#ifndef _WIN32

	if (::isatty(this->fd)) {
		//get the terminal settings for stdin
		::tcgetattr(this->fd, &this->old_tio);
		//backup old settings
		struct termios new_tio = this->old_tio;
		//disable buffered i/o (canonical mode) and local echo
		new_tio.c_lflag &= (~ICANON & ~ECHO & ~ISIG);
		//set the settings
		::tcsetattr(this->fd, TCSANOW, &new_tio);
		this->restore_input_mode_on_destroy = true;
	}

	#endif /* _WIN32 */
}

void FD::restoreinputmode() {
	#ifndef _WIN32

	if (::isatty(this->fd)) {
		::tcsetattr(this->fd, TCSANOW, &this->old_tio);
		this->restore_input_mode_on_destroy = false;
	}

	#endif /* _WIN32 */
}

} // openage::util
