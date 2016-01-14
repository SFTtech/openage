// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_CROSSPLATFORM_PTY_H_
#define OPENAGE_CROSSPLATFORM_PTY_H_

#ifdef __APPLE__
#  include <util.h>
#elif _WIN32
//TODO not yet implemented
#else
#  include <pty.h>
#endif

#endif
