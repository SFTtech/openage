// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_CROSSPLATFORM_SIGNAL_H_
#define OPENAGE_CROSSPLATFORM_SIGNAL_H_

#include <signal.h>

// TODO: change these to ifndef __linux ?
#ifdef __APPLE__
	typedef void (*sighandler_t)(int);
#endif
#ifdef _WIN32
	typedef void (*sighandler_t)(int);
#endif

#endif
