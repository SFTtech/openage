// Copyright 2014-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <signal.h>

// \todo change these to ifndef __linux ?
#ifdef __APPLE__
	typedef void (*sighandler_t)(int);
#endif
#ifdef _WIN32
	typedef void (*sighandler_t)(int);
#endif
