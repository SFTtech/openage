#ifndef _ENGINE_UTIL_FDS_H_
#define _ENGINE_UTIL_FDS_H_

#include <stdlib.h>
#include <unistd.h>
#include <termio.h>

namespace openage {
namespace util {

/**
 * wraps a POSIX file descriptor
 */
class FD {
public:
	/**
	 * wraps an existing FD
	 */
	FD(int fd=STDOUT_FILENO);

	/**
	 * duplicates an existing FD, and optionally
	 * sets it to non-blocking mode
	 */
	FD(int fd, bool set_nonblocking);
	FD(FD *fd, bool set_nonblocking);

	~FD();

	int fd;

	/**
	 * if this is set to true, the destructor will close the fd.
	 * will be set to true by the constructor iff the fd was duped.
	 */
	bool close_on_destroy = false;

	/**
	 * if this is set to true, the destructor will restore the input
	 * mode. will be set on setinputmodecanon().
	 */
	bool restore_input_mode_on_destroy = false;

	/**
	 * writes 'bytes' bytes from buf
	 */
	int write(const char *buf, size_t bytes);

	/**
	 * writes the string (excluding the null terminator)
	 */
	int puts(const char *str);

	/**
	 * writes the char
	 */
	int putbyte(char c);

	/**
	 * writes the unicode codepoint, as utf-8
	 */
	int putcp(int cp);

	/**
	 * guess what this does. because I won't tell you.
	 */
	int printf(const char *format, ...);

	/**
	 * sets input to canonical mode.
	 * this includes:
	 * - disabling ECHO
	 * - non-buffered input
	 * - no generation of signals on ^C, ^Z, ...
	 */
	void setinputmodecanon();

	/**
	 * restores the input mode to the mode that was
	 * backed up during the last setinputmodecanon() mode.
	 */
	void restoreinputmode();

	struct termios old_tio;
};

} //namespace util
} //namespace openage

#endif //_ENGINE_UTIL_FDS_H_
