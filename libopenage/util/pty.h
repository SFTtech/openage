// Copyright 2014-2019 the openage authors. See copying.md for legal info.

#pragma once

#ifdef __APPLE__
#  include <util.h>
#elif defined(__FreeBSD__)
#  include <sys/types.h>
#  include <sys/ioctl.h>
#  include <termios.h>
#  include <libutil.h>
#elif _WIN32
// \todo not yet implemented
#else
#  include <pty.h>
#endif
