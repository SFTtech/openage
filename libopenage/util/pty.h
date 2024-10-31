// Copyright 2014-2024 the openage authors. See copying.md for legal info.

#pragma once

#ifdef __APPLE__
	#include <util.h>
#elif defined(__FreeBSD__)
	#include <libutil.h>
	#include <sys/ioctl.h>
	#include <sys/types.h>
	#include <termios.h>
#elif _WIN32
// TODO not yet implemented
#else
	#include <pty.h>
#endif
